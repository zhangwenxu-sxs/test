#include <linux/platform_device.h>

#include "lt8522ex_main.h"
#include "lt8522ex_misc.h"
#include "sn_netlink.h"

//描述lt8522ex内部集成的I2C设备的地址
static struct i2c_board_info __initdata lt8522ex_info[] = {
    {
        I2C_BOARD_INFO("lt8522ex_addr0", 0x50 >> 1),
    },
    {
        I2C_BOARD_INFO("lt8522ex_addr1", 0x52 >> 1),
    },
    {
        I2C_BOARD_INFO("lt8522ex_addr2", 0x54 >> 1),
    },
    {
        I2C_BOARD_INFO("lt8522ex_addr3", 0x56 >> 1),
    }
};

static int lt8522ex_cnt;          //板上的lt8522ex总数
static dev_t lt8522ex_dev_num;          //分配给lt8522ex使用的设备号
static struct cdev lt8522ex_chrdev;     //给用户空间提供的字符设备
static struct class* lt8522ex_class;    //

lt8522ex_t* curr_lt8522ex;              //当前操作的lt8522ex
struct i2c_client* curr_client;         //当前所操作的i2c client

/**
 * lt8522ex框图
 *        |----------|
 *HDMI--->|          |--->HDMI
 *        |   8522   |
 * VGA--->|          |--->VGA
 *        |----------|
 * */

lt8522ex_t lt8522exs[MAX_LT8522EX_CNT];     //用作输出的lt8522ex
//用于轮询lt8522ex状态并做相关配置的线程
//注: lt8522ex的中断信号有问题, 目前只能用轮询方式更新状态
static struct task_struct* poll_task = NULL;
//这个变量用于控制线程的退出
static int running = 1;

static void lt8522ex_reset(lt8522ex_t* lt8522ex);   //复位lt8522ex
static void lt8522ex_poll(lt8522ex_t* lt8522ex);    //轮询lt8522ex的寄存器
static int lt8522ex_detect_thread(void* data);      //用于检测lt8522ex状态的线程

static int lt8522ex_open(struct inode* inode, struct file* file);
static int lt8522ex_release(struct inode* inode, struct file* file);
static long lt8522ex_ioctl(struct file* file, unsigned int cmd, unsigned long args);

static struct file_operations lt8522ex_fops = {
    .open = lt8522ex_open,
    .release = lt8522ex_release,
    .unlocked_ioctl = lt8522ex_ioctl
};

static void lt8522ex_reset(lt8522ex_t* lt8522ex)
{
    if (lt8522ex->reset_pin == INVALID_GPIO)
        return;

    gpio_direction_output(lt8522ex->reset_pin, 0);
    usleep_range(10000, 11000);
    gpio_direction_output(lt8522ex->reset_pin, 1);
    usleep_range(10000, 11000);
}

static void lt8522ex_poll(lt8522ex_t* lt8522ex)
{
    char nl_msg[128];           //netlink消息
    curr_lt8522ex = lt8522ex;   //切换当前操作的lt8522ex
    curr_lt8522ex->event = 0;   //清零事件标志

    TxHpdChangeDetect();
    TxVgaChangeDetect();
    RxHdmi5vDetect();

    //如果用户配置了输入输出对, 那么更新相关寄存器
    if (curr_lt8522ex->pair_update) {
        curr_lt8522ex->pair_update = 0;
        curr_lt8522ex->pair1 = curr_lt8522ex->tmp_pair1;
        curr_lt8522ex->pair2 = curr_lt8522ex->tmp_pair2;
        LT8522EX_CONFIG_INPUT_OUTPUT();
    }

    //如果用户配置了输入接口的EDID, 则更新EDID
    if (curr_lt8522ex->vga_in_edid_update) {
        lt8522ex->vga_in_edid_update = 0;
        LT8522EX_RxPowerDown(VGA);  //先关闭VGA输入模块
        RebuildSinkEdid(VGA, VGA);  //重组EDID
        SaveEdidToShadow(VGA);      //将处理后的EDID写入VGA输入接口对应的内存
    } else if(curr_lt8522ex->hdmi_in_edid_update) {
        curr_lt8522ex->hdmi_in_edid_update = 0;
        LT8522EX_RxPowerDown(HDMI); //先关闭HDMI输入模块
        RebuildSinkEdid(HDMI, HDMI);//重组EDID
        SaveEdidToShadow(HDMI);     //将处理后的EDID写入HDMI输入接口对应的内存
    }

    //VGA输出口有显示设备接入，获取VGA输出接口所接显示设备的EDID
    if (curr_lt8522ex->FlagVgaDetChange) {
        if (curr_lt8522ex->hdmi_in_hpd_pin != INVALID_GPIO)
            gpio_direction_output(curr_lt8522ex->hdmi_in_hpd_pin, 0);

        Delay_ms(300);

        mutex_lock(&curr_lt8522ex->edid_mutex);
        GetTxEdid(VgaSink);
        mutex_unlock(&curr_lt8522ex->edid_mutex);

        curr_lt8522ex->FlagTxVgaStartWork = 1;
        curr_lt8522ex->FlagVgaDetChange = 0;
    }

    //检测到HDMI输出口有显示设备接入，获取HDMI输出接口所接显示设备的EDID
    if (curr_lt8522ex->FlagTxHpdChange) {
        if (curr_lt8522ex->hdmi_in_hpd_pin != INVALID_GPIO)
            gpio_direction_output(curr_lt8522ex->hdmi_in_hpd_pin, 0);

        Delay_ms(300);

        mutex_lock(&curr_lt8522ex->edid_mutex);
        GetTxEdid(HdmiSink);
        mutex_unlock(&curr_lt8522ex->edid_mutex);

        curr_lt8522ex->FlagTxHdmiStartWork = 1;
        curr_lt8522ex->FlagTxHpdChange = 0;
    }

    //如果将输入源配置成了HDMI
    if ((curr_lt8522ex->pair1 & LT8522EX_INPUT_MASK) == LT8522EX_HDMI_INPUT ||
        (curr_lt8522ex->pair2 & LT8522EX_INPUT_MASK) == LT8522EX_HDMI_INPUT) {
//        lt8522ex_dbg("%s: use HDMI IN as source \n", curr_lt8522ex->name);
        //检测当前有没有HDMI输入源
        if (!curr_lt8522ex->FlagRxHdmi5V) {
            if (curr_lt8522ex->hdmi_in_hpd_pin != INVALID_GPIO)
                gpio_direction_output(curr_lt8522ex->hdmi_in_hpd_pin, 0);

            Delay_ms(300);
            LT8522EX_RxPowerDown(HDMI);
        } else {
            LT8522EX_ClockEnable_HDMI();
            LT8522EX_RxPowerOn(HDMI);
        }

        //如果检测到HDMI输入接口有设备接入，同时VGA输出接口或HDMI输出接口有设备接入
        if (curr_lt8522ex->FlagRxHdmi5VRise || curr_lt8522ex->FlagTxVgaStartWork || curr_lt8522ex->FlagTxHdmiStartWork) {
            curr_lt8522ex->FlagRxHdmi5VRise = 0;
            if ((!curr_lt8522ex->FlagTxHdmiOff) || (!curr_lt8522ex->FlagTxVgaOff)) {
                if (curr_lt8522ex->FlagTxVgaStartWork || curr_lt8522ex->FlagTxHdmiStartWork) {
                    mutex_lock(&curr_lt8522ex->edid_mutex);
                    BuildSourceEdid(HDMI);  //根据输入输出口的配置情况，重新组合EDID
                    SaveEdidToShadow(HDMI); //将EDID写入相应的寄存器，以供源端设备获取(这个函数内部调用有问题)
                    mutex_unlock(&curr_lt8522ex->edid_mutex);
                }
                //拉高HPD, 源端可以开始获取EDID
                if (curr_lt8522ex->FlagRxHdmi5V) {
                    if (curr_lt8522ex->hdmi_in_hpd_pin != INVALID_GPIO)
                        gpio_direction_output(curr_lt8522ex->hdmi_in_hpd_pin, 1);   //给源端发送HPD信号
                    LT8522EX_ColorConvert(HDMI);    //根据输入信号的颜色空间、位宽，配置相关寄存器
                }
            }
        }
    }

    //如果输入源配置成了VGA
    if ((curr_lt8522ex->pair1 & LT8522EX_INPUT_MASK) == LT8522EX_VGA_INPUT ||
        (curr_lt8522ex->pair2 & LT8522EX_INPUT_MASK) == LT8522EX_VGA_INPUT) {
//        lt8522ex_dbg("%s: use VGA IN as source \n", curr_lt8522ex->name);
        //检测VGA输入时序的变更
        RxVgaChangeDetect();

        // 根据输入和输出接口配置的情况配置EDID(这段代码默认被厂家屏蔽)
        if (curr_lt8522ex->FlagTxVgaStartWork || curr_lt8522ex->FlagTxHdmiStartWork) {
            mutex_lock(&curr_lt8522ex->edid_mutex);
            BuildSourceEdid(VGA);       //把从输出端口获得的显示设备的EDID拷贝到输入端口的EDID内存
            SaveEdidToShadow(VGA);
            mutex_unlock(&curr_lt8522ex->edid_mutex);
        }

        if (curr_lt8522ex->FlagVgaTimingChange) {
            curr_lt8522ex->FlagVgaTimingChange = 0;

            if ((curr_lt8522ex->videoformat != SignnalOff) && (curr_lt8522ex->videoformat != VESA_Nonstandard)) {
                //if(VGA2HDMI&&(!FlagTxHdmiOff))
                {
                    curr_client = curr_lt8522ex->client_addr0;
                    HDMI_WriteI2C_Byte(0x44, 0x9f); //ECO
                    LT8522EX_TxDriveConfig(DriveOff);
                    LT8522EX_AdcReset();
                    LT8522EX_AdcPllDiv();
                    LT8522EX_AdcPllInit();
                    LT8522EX_SetSyncPol();
//                    LT8522EX_RGBClamp();
//                    LT8522EX_VrtopSelect();
                    LT8522EX_SetTimingReg();
                    LT8522EX_InterruptClear();
                    LT8522EX_SetTimingWindow();
                    LT8522EX_AutoPiPhase();

                    curr_client = curr_lt8522ex->client_addr0;
                    HDMI_WriteI2C_Byte(0x44, 0x8f); //close RGB clamp,after ECO bit4 0f 0x44 is clamp enable
                }
            }
        }
    }

    //配置音频输入输出相关的东西
    LT8522EX_AudioConfig(HDMI);
    LT8522EX_AudioConfig(VGA);

    if (((curr_lt8522ex->pair1 == (LT8522EX_HDMI_INPUT | LT8522EX_VGA_OUTPUT)) &&
         (curr_lt8522ex->pair2 == (LT8522EX_VGA_INPUT | LT8522EX_HDMI_OUTPUT))) ||
        ((curr_lt8522ex->pair2 == (LT8522EX_HDMI_INPUT | LT8522EX_VGA_OUTPUT)) &&
         (curr_lt8522ex->pair1 == (LT8522EX_VGA_INPUT | LT8522EX_HDMI_OUTPUT)))) {
        LT8522EX_RxPowerOn(VGA);
        LT8522EX_RxPowerOn(HDMI);
        LT8522EX_ColorConvert(HDMI);
    } else if (((curr_lt8522ex->pair1 == (LT8522EX_HDMI_INPUT | LT8522EX_VGA_OUTPUT)) &&
                (curr_lt8522ex->pair2 == (LT8522EX_HDMI_INPUT | LT8522EX_HDMI_OUTPUT))) ||
                ((curr_lt8522ex->pair2 == (LT8522EX_HDMI_INPUT | LT8522EX_VGA_OUTPUT)) &&
                (curr_lt8522ex->pair1 == (LT8522EX_HDMI_INPUT | LT8522EX_HDMI_OUTPUT)))) {
        LT8522EX_RxPowerOn(HDMI);
        LT8522EX_RxPowerDown(VGA);
        LT8522EX_ColorConvert(HDMI);
    } else if (((curr_lt8522ex->pair1 == (LT8522EX_VGA_INPUT | LT8522EX_VGA_OUTPUT)) &&
                (curr_lt8522ex->pair2 == (LT8522EX_HDMI_INPUT | LT8522EX_HDMI_OUTPUT))) ||
                ((curr_lt8522ex->pair2 == (LT8522EX_VGA_INPUT | LT8522EX_VGA_OUTPUT)) &&
                (curr_lt8522ex->pair1 == (LT8522EX_HDMI_INPUT | LT8522EX_HDMI_OUTPUT)))) {
        LT8522EX_RxPowerOn(VGA);
        LT8522EX_RxPowerOn(HDMI);
        LT8522EX_ColorConvert(VGA);
    } else if (((curr_lt8522ex->pair1 == (LT8522EX_VGA_INPUT | LT8522EX_VGA_OUTPUT)) &&
                (curr_lt8522ex->pair2 == (LT8522EX_VGA_INPUT | LT8522EX_HDMI_OUTPUT))) ||
                ((curr_lt8522ex->pair2 == (LT8522EX_VGA_INPUT | LT8522EX_VGA_OUTPUT)) &&
                (curr_lt8522ex->pair1 == (LT8522EX_VGA_INPUT | LT8522EX_HDMI_OUTPUT)))) {
        LT8522EX_RxPowerOn(VGA);
        LT8522EX_RxPowerDown(HDMI);
        LT8522EX_ColorConvert(VGA);
    }

    if (curr_lt8522ex->event != 0) {
        //更新sys文件系统中相关文件状态
        if (curr_lt8522ex->event & EVENT_HDMI_IN_PLUG_IN) {
            switch_set_state(&curr_lt8522ex->sw_hdmi_in, 1);
        } else if(curr_lt8522ex->event & EVENT_HDMI_IN_PLUG_OUT) {
            switch_set_state(&curr_lt8522ex->sw_hdmi_in, 0);
        } else if(curr_lt8522ex->event & EVENT_VGA_IN_PLUG_IN) {
            switch_set_state(&curr_lt8522ex->sw_vga_in, 1);
        } else if(curr_lt8522ex->event & EVENT_VGA_IN_PLUG_OUT) {
            switch_set_state(&curr_lt8522ex->sw_vga_in, 0);
        } else if(curr_lt8522ex->event & EVENT_HDMI_OUT_PLUG_IN) {
            switch_set_state(&curr_lt8522ex->sw_hdmi_out, 1);
        } else if(curr_lt8522ex->event & EVENT_HDMI_OUT_PLUG_OUT) {
            switch_set_state(&curr_lt8522ex->sw_hdmi_out, 0);
        } else if(curr_lt8522ex->event & EVENT_VGA_OUT_PLUG_IN) {
            switch_set_state(&curr_lt8522ex->sw_vga_out, 1);
        } else if(curr_lt8522ex->event & EVENT_VGA_OUT_PLUG_OUT) {
            switch_set_state(&curr_lt8522ex->sw_vga_out, 0);
        }

        curr_lt8522ex->event_bak = curr_lt8522ex->event;
        //向应用上报netlink消息, 应用接收到消息后通过ioctl获取详细事件
        sprintf(nl_msg, LT8522EX_NL_MSG_FMT, curr_lt8522ex->name);
        post_nl_msg(nl_msg, STAR_NET_DRIVER_MSG_GRP);
    }

    curr_lt8522ex->FlagTxVgaStartWork = 0;
    curr_lt8522ex->FlagTxHdmiStartWork = 0;
}

static int lt8522ex_detect_thread(void* data)
{
    int i;
    lt8522ex_dbg("poll thread start... \n");

    while (running) {
        for (i = 0; i < lt8522ex_cnt; i++) {
            lt8522ex_poll(&lt8522exs[i]);
        }

        usleep_range(50000, 51000);
    }

    return 0;
}

static int lt8522ex_open(struct inode* inode, struct file* file)
{
    if (MINOR(inode->i_rdev) >= lt8522ex_cnt) {
        lt8522ex_log("invalid minor number \n");
        return -1;
    }

    file->private_data = &lt8522exs[MINOR(inode->i_rdev)];

    return 0;
}

static int lt8522ex_release(struct inode* inode, struct file* file)
{
    return 0;
}

static long lt8522ex_ioctl(struct file* file, unsigned int cmd, unsigned long args)
{
    unsigned long count;
    lt8522ex_t* lt8522ex = file->private_data;

    switch (cmd) {
    case CMD_GET_VGA_OUTPUT_EDID:
        mutex_lock(&lt8522ex->edid_mutex);
        count = copy_to_user((void*)args, lt8522ex->VgaSinkEdid, 128);
        mutex_unlock(&lt8522ex->edid_mutex);
        return count == 0 ? 0 : -1;
    case CMD_GET_HDMI_OUTPUT_EDID:
        mutex_lock(&lt8522ex->edid_mutex);
        count = copy_to_user((void*)args, lt8522ex->HdmiSinkEdid, 256);
        mutex_unlock(&lt8522ex->edid_mutex);
        return count == 0 ? 0 : -1;
    case CMD_SET_VGA_INPUT_EDID:
        mutex_lock(&lt8522ex->edid_mutex);
        count = copy_from_user((void*)lt8522ex->VgaSinkEdid, (void*)args, 128);
        mutex_unlock(&lt8522ex->edid_mutex);
        lt8522ex->vga_in_edid_update = 1;
        break;
    case CMD_SET_HDMI_INPUT_EDID:
        mutex_lock(&lt8522ex->edid_mutex);
        count = copy_from_user((void*)lt8522ex->HdmiSinkEdid, (void*)args, 256);
        mutex_unlock(&lt8522ex->edid_mutex);
        lt8522ex->hdmi_in_edid_update = 1;
        break;
    case CMD_GET_LT8522EX_EVENT:
        count = copy_to_user((void*)args, &lt8522ex->event_bak, sizeof(lt8522ex->event_bak));
        return count == 0 ? 0 : -1;
    case CMD_SET_INPUT_OUTPUT_PAIR:
        lt8522ex->tmp_pair1 = ((input_output_pair_config_t*)args)->pair1;
        lt8522ex->tmp_pair2 = ((input_output_pair_config_t*)args)->pair2;
        lt8522ex->pair_update = 1;
        break;
    }

    return 0;
}

static void lt8522ex_request_gpio(lt8522ex_t* lt8522ex)
{
    if (lt8522ex->hdmi_in_detect_pin != INVALID_GPIO)
        gpio_request(lt8522ex->hdmi_in_detect_pin, NULL);
    if (lt8522ex->hdmi_in_hpd_pin != INVALID_GPIO)
        gpio_request(lt8522ex->hdmi_in_hpd_pin, NULL);
    if (lt8522ex->vga_in_detect_pin != INVALID_GPIO)
        gpio_request(lt8522ex->vga_in_detect_pin, NULL);
    if (lt8522ex->vga_out_detect_pin != INVALID_GPIO)
        gpio_request(lt8522ex->vga_out_detect_pin, NULL);
    if (lt8522ex->reset_pin != INVALID_GPIO)
        gpio_request(lt8522ex->reset_pin, NULL);
}

static void lt8522ex_free_gpio(lt8522ex_t* lt8522ex)
{
    if (lt8522ex->hdmi_in_detect_pin != INVALID_GPIO)
        gpio_free(lt8522ex->hdmi_in_detect_pin);
    if (lt8522ex->hdmi_in_hpd_pin != INVALID_GPIO)
        gpio_free(lt8522ex->hdmi_in_hpd_pin);
    if (lt8522ex->vga_in_detect_pin != INVALID_GPIO)
        gpio_free(lt8522ex->vga_in_detect_pin);
    if (lt8522ex->vga_out_detect_pin != INVALID_GPIO)
        gpio_free(lt8522ex->vga_out_detect_pin);
    if (lt8522ex->reset_pin != INVALID_GPIO)
        gpio_free(lt8522ex->reset_pin);
}

static int lt8522ex_probe(struct platform_device* pdev)
{
    int ret;
    char* tmp;
    unsigned int i2c_bus;       //lt8522ex所接的总线号
    struct device_node* child;  //用于遍历所有lt8522ex节点的指针

    int idx;
    unsigned int val;
    unsigned int reg_addr;
    volatile unsigned int* reg_ptr;

    lt8522ex_log("start probe lt8522ex in board \n");
    if (pdev->dev.of_node == NULL) {
        lt8522ex_log("not device tree node associated with this device \n");
        return -1;
    }

    //动态分配字符设备号
    ret = alloc_chrdev_region(&lt8522ex_dev_num, 0, MAX_LT8522EX_CNT, "lt8522ex_region");
    if (ret != 0) {
        lt8522ex_log("alloc character device number failed \n");
        goto err1;
    }

    //初始化并注册字符设备
    cdev_init(&lt8522ex_chrdev, &lt8522ex_fops);
    ret = cdev_add(&lt8522ex_chrdev, lt8522ex_dev_num, MAX_LT8522EX_CNT);
    if (ret !=0 ) {
        lt8522ex_log("register character device to kernel failed \n");
        goto err2;
    }

    //创建用户空间的字符设备节点
    lt8522ex_class = class_create(THIS_MODULE, "lt8522ex_class");
    if (IS_ERR(lt8522ex_class)) {
        lt8522ex_log("create lt8522ex_class failed \n");
        goto err3;
    }

    //创建轮询线程
    poll_task = kthread_create(lt8522ex_detect_thread, NULL, "lt8522ex detect thread");
    if (poll_task == NULL) {
        lt8522ex_log("create lt8522ex_detect_thread failed \n");
        goto err4;
    }

    //遍历所有的lt8522ex节点
    for_each_child_of_node(pdev->dev.of_node, child) {
        lt8522ex_dbg("parse device node[%s] \n", child->full_name);
        //读取寄存器配置属性并完成相关配置(该功能暂不可用, ioremap会返回0, 还未定位出原因)
        idx = 0;
        while (1) {
            ret = of_property_read_u32_index(child, "reg_cfg", idx, (unsigned int*)&reg_addr);
            if (ret != 0) {
                break;
            }

            idx += 1;

            ret = of_property_read_u32_index(child, "reg_cfg", idx, &val);
            if (ret != 0) {
                break;
            }

            idx += 1;

            reg_ptr = (volatile unsigned int*)ioremap(reg_addr, 4);
            if (reg_ptr != NULL) {
                *reg_ptr = val;
                iounmap((void*)reg_ptr);
                lt8522ex_dbg("set reg[0x%x] to val[0x%x] \n", reg_addr, val);
            }
        }

        //读取lt8522ex的名字
        ret = of_property_read_string(child, "devname", (const char**)(&lt8522exs[lt8522ex_cnt].name));
        if (ret != 0) {
            lt8522ex_log("get property[%s] failed \n", "devname");
            continue;
        }

        //获取lt8522ex的总线地址
        ret = of_property_read_u32(child, "i2c_info", &i2c_bus);
        if (ret != 0) {
            lt8522ex_log("get number of i2c bus failed \n");
            continue;
        }

        //获取检测HDMI输入接口是否有输入源接入的GPIO
        ret = of_property_read_u32(child, "hdmi_in_detect_pin", &lt8522exs[lt8522ex_cnt].hdmi_in_detect_pin);
        if (ret != 0) {
            lt8522ex_log("get hdmi_in_detect_pin failed \n");
        }

        if (ret != 0 || lt8522exs[lt8522ex_cnt].hdmi_in_detect_pin == 0xff) {
            lt8522exs[lt8522ex_cnt].hdmi_in_detect_pin = INVALID_GPIO;
        }

        //获取控制HDMI输入接口HPD信号的GPIO
        ret = of_property_read_u32(child, "hdmi_in_hpd_pin", &lt8522exs[lt8522ex_cnt].hdmi_in_hpd_pin);
        if (ret != 0) {
            lt8522ex_log("get hdmi_in_hpd_pin failed \n");
        }

        if (ret != 0 || lt8522exs[lt8522ex_cnt].hdmi_in_hpd_pin == 0xff) {
            lt8522exs[lt8522ex_cnt].hdmi_in_hpd_pin = INVALID_GPIO;
        }

        //获取检测VGA输入接口是否有输入源接入的GPIO
        ret = of_property_read_u32(child, "vga_in_detect_pin", &lt8522exs[lt8522ex_cnt].vga_in_detect_pin);
        if (ret != 0) {
            lt8522ex_log("get vga_in_detect_pin failed \n");
        }

        if (ret != 0 || lt8522exs[lt8522ex_cnt].vga_in_detect_pin == 0xff) {
            lt8522exs[lt8522ex_cnt].vga_in_detect_pin = INVALID_GPIO;
        }

        //获取检测VGA输出接口是否有显示设备接入的GPIO
        ret = of_property_read_u32(child, "vga_out_detect_pin", &lt8522exs[lt8522ex_cnt].vga_out_detect_pin);
        if (ret != 0) {
            lt8522ex_log("get vga_out_detect_pin failed \n");
        }

        if (ret != 0 || lt8522exs[lt8522ex_cnt].vga_out_detect_pin == 0xff) {
            lt8522exs[lt8522ex_cnt].vga_out_detect_pin = INVALID_GPIO;
        }

        //获取控制lt8522ex芯片复位的GPIO
        ret = of_property_read_u32(child, "reset_pin", &lt8522exs[lt8522ex_cnt].reset_pin);
        if (ret != 0) {
            lt8522ex_log("get reset_pin failed \n");
        }

        if (ret != 0 || lt8522exs[lt8522ex_cnt].reset_pin == 0xff) {
            lt8522exs[lt8522ex_cnt].reset_pin = INVALID_GPIO;
        }

        //创建字符设备号
        lt8522exs[lt8522ex_cnt].dev = MKDEV(MAJOR(lt8522ex_dev_num), lt8522ex_cnt);
        //初始化switch设备名
        tmp = (char*)kzalloc(SWITCH_NAME_BUFF_SIZE, GFP_KERNEL);
        if (tmp != NULL) {
            sprintf(tmp, "%s_hdmi_in", lt8522exs[lt8522ex_cnt].name);
            lt8522exs[lt8522ex_cnt].sw_hdmi_in.name = tmp;
        }
        tmp = (char*)kzalloc(SWITCH_NAME_BUFF_SIZE, GFP_KERNEL);
        if (tmp != NULL) {
            sprintf(tmp, "%s_vga_in", lt8522exs[lt8522ex_cnt].name);
            lt8522exs[lt8522ex_cnt].sw_vga_in.name = tmp;
        }
        tmp = (char*)kzalloc(SWITCH_NAME_BUFF_SIZE, GFP_KERNEL);
        if (tmp != NULL) {
            sprintf(tmp, "%s_hdmi_out", lt8522exs[lt8522ex_cnt].name);
            lt8522exs[lt8522ex_cnt].sw_hdmi_out.name = tmp;
        }
        tmp = (char*)kzalloc(SWITCH_NAME_BUFF_SIZE, GFP_KERNEL);
        if (tmp != NULL) {
            sprintf(tmp, "%s_vga_out", lt8522exs[lt8522ex_cnt].name);
            lt8522exs[lt8522ex_cnt].sw_vga_out.name = tmp;
        }
        //初始化默认的输入输出配置
        //默认为HDMI IN作为输入源， HDMI OUT 和 VGA OUT同源输出
        lt8522exs[lt8522ex_cnt].pair1 = LT8522EX_HDMI_INPUT | LT8522EX_HDMI_OUTPUT;
        lt8522exs[lt8522ex_cnt].pair2 = LT8522EX_HDMI_INPUT | LT8522EX_VGA_OUTPUT;
        //初始化一些更新标志
        lt8522exs[lt8522ex_cnt].pair_update = 0;
        lt8522exs[lt8522ex_cnt].vga_in_edid_update = 0;
        lt8522exs[lt8522ex_cnt].hdmi_in_edid_update = 0;
        //初始化一些状态变量
        if (lt8522exs[lt8522ex_cnt].hdmi_in_detect_pin == INVALID_GPIO) {
            lt8522exs[lt8522ex_cnt].FlagRxHdmi5V = 1;       //HDMI IN始终连接
            lt8522exs[lt8522ex_cnt].FlagRxHdmi5VFall = 0;
            lt8522exs[lt8522ex_cnt].FlagRxHdmi5VRise = 1;
        } else {
            lt8522exs[lt8522ex_cnt].FlagRxHdmi5V = 0;
            lt8522exs[lt8522ex_cnt].FlagRxHdmi5VFall = 0;
            lt8522exs[lt8522ex_cnt].FlagRxHdmi5VRise = 0;
        }

        lt8522exs[lt8522ex_cnt].FlagTxHpdChange = 0;
        lt8522exs[lt8522ex_cnt].FlagVgaDetChange = 0;
        lt8522exs[lt8522ex_cnt].FlagStartWork = 0;
        lt8522exs[lt8522ex_cnt].FlagTxHdmiOff = 1;
        lt8522exs[lt8522ex_cnt].FlagTxVgaOff = 1;
        lt8522exs[lt8522ex_cnt].FlagTxChangeRx = 0;
        lt8522exs[lt8522ex_cnt].OldTxPlugStatus = 0x00;
        lt8522exs[lt8522ex_cnt].OldVgaDetStatus = 0x00;
        lt8522exs[lt8522ex_cnt].videoformat = SignnalOff;
        lt8522exs[lt8522ex_cnt].LastVideoFormat = SignnalOff;

        //申请GPIO资源
        lt8522ex_request_gpio(&lt8522exs[lt8522ex_cnt]);
        //初始化互斥锁
        mutex_init(&lt8522exs[lt8522ex_cnt].edid_mutex);

        //创建i2c相关资源
        lt8522exs[lt8522ex_cnt].adapter = i2c_get_adapter(i2c_bus);
        if (lt8522exs[lt8522ex_cnt].adapter == NULL) {
            lt8522ex_log("get i2c-%d adapter failed \n", i2c_bus);
            //错误处理
            lt8522ex_free_gpio(&lt8522exs[lt8522ex_cnt]);
            continue;
        }
        lt8522exs[lt8522ex_cnt].client_addr0 = i2c_new_device(lt8522exs[lt8522ex_cnt].adapter, &lt8522ex_info[0]);
        if (lt8522exs[lt8522ex_cnt].client_addr0 == NULL) {
            lt8522ex_log("create i2c client for addr[0x%x] failed \n", lt8522ex_info[0].addr);
            //错误处理
            i2c_put_adapter(lt8522exs[lt8522ex_cnt].adapter);
            lt8522ex_free_gpio(&lt8522exs[lt8522ex_cnt]);
            continue;
        }

        lt8522exs[lt8522ex_cnt].client_addr1 = i2c_new_device(lt8522exs[lt8522ex_cnt].adapter, &lt8522ex_info[1]);
        if (lt8522exs[lt8522ex_cnt].client_addr1 == NULL) {
            lt8522ex_log("create i2c client for addr[0x%x] failed \n", lt8522ex_info[1].addr);
            //错误处理
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr0);
            i2c_put_adapter(lt8522exs[lt8522ex_cnt].adapter);
            lt8522ex_free_gpio(&lt8522exs[lt8522ex_cnt]);
            continue;
        }

        lt8522exs[lt8522ex_cnt].client_addr2 = i2c_new_device(lt8522exs[lt8522ex_cnt].adapter, &lt8522ex_info[2]);
        if (lt8522exs[lt8522ex_cnt].client_addr2 == NULL) {
            lt8522ex_log("create i2c client for addr[0x%x] failed \n", lt8522ex_info[2].addr);
            //错误处理
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr1);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr0);
            i2c_put_adapter(lt8522exs[lt8522ex_cnt].adapter);
            lt8522ex_free_gpio(&lt8522exs[lt8522ex_cnt]);
            continue;
        }

        lt8522exs[lt8522ex_cnt].client_addr3 = i2c_new_device(lt8522exs[lt8522ex_cnt].adapter, &lt8522ex_info[3]);
        if (lt8522exs[lt8522ex_cnt].client_addr3 == NULL) {
            lt8522ex_log("create i2c client for addr[0x%x] failed \n", lt8522ex_info[3].addr);
            //错误处理
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr2);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr1);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr0);
            i2c_put_adapter(lt8522exs[lt8522ex_cnt].adapter);
            lt8522ex_free_gpio(&lt8522exs[lt8522ex_cnt]);
            continue;
        }

        //创建上报状态的switch设备
        ret = switch_dev_register(&lt8522exs[lt8522ex_cnt].sw_hdmi_in);
        if (ret != 0) {
            lt8522ex_log("register switch device for lt8522ex_input failed \n");
            //错误处理
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr3);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr2);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr1);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr0);
            i2c_put_adapter(lt8522exs[lt8522ex_cnt].adapter);
            lt8522ex_free_gpio(&lt8522exs[lt8522ex_cnt]);
            continue;
        }

        ret = switch_dev_register(&lt8522exs[lt8522ex_cnt].sw_vga_in);
        if (ret != 0) {
            lt8522ex_log("register switch device for lt8522ex_input failed \n");
            //错误处理
            switch_dev_unregister(&lt8522exs[lt8522ex_cnt].sw_hdmi_in);
            kfree(lt8522exs[lt8522ex_cnt].sw_hdmi_in.name);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr3);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr2);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr1);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr0);
            i2c_put_adapter(lt8522exs[lt8522ex_cnt].adapter);
            lt8522ex_free_gpio(&lt8522exs[lt8522ex_cnt]);
            continue;
        }

        ret = switch_dev_register(&lt8522exs[lt8522ex_cnt].sw_hdmi_out);
        if (ret != 0) {
            lt8522ex_log("register switch device for lt8522ex_output failed \n");
            //错误处理
            switch_dev_unregister(&lt8522exs[lt8522ex_cnt].sw_vga_in);
            kfree(lt8522exs[lt8522ex_cnt].sw_vga_in.name);
            switch_dev_unregister(&lt8522exs[lt8522ex_cnt].sw_hdmi_in);
            kfree(lt8522exs[lt8522ex_cnt].sw_hdmi_in.name);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr3);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr2);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr1);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr0);
            i2c_put_adapter(lt8522exs[lt8522ex_cnt].adapter);
            lt8522ex_free_gpio(&lt8522exs[lt8522ex_cnt]);
            continue;
        }

        ret = switch_dev_register(&lt8522exs[lt8522ex_cnt].sw_vga_out);
        if (ret != 0) {
            lt8522ex_log("register switch device for lt8522ex_output failed \n");
            //错误处理
            switch_dev_unregister(&lt8522exs[lt8522ex_cnt].sw_hdmi_out);
            kfree(lt8522exs[lt8522ex_cnt].sw_hdmi_out.name);
            switch_dev_unregister(&lt8522exs[lt8522ex_cnt].sw_vga_in);
            kfree(lt8522exs[lt8522ex_cnt].sw_vga_in.name);
            switch_dev_unregister(&lt8522exs[lt8522ex_cnt].sw_hdmi_in);
            kfree(lt8522exs[lt8522ex_cnt].sw_hdmi_in.name);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr3);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr2);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr1);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr0);
            i2c_put_adapter(lt8522exs[lt8522ex_cnt].adapter);
            lt8522ex_free_gpio(&lt8522exs[lt8522ex_cnt]);
            continue;
        }

        //复位lt8522ex
        lt8522ex_reset(&lt8522exs[lt8522ex_cnt]);
        usleep_range(100000, 110000);
        //初始化lt8522ex相关寄存器
        curr_lt8522ex = &lt8522exs[lt8522ex_cnt];
        LT8522EX_Init();
        //创建对应的字符设备
        if (IS_ERR(device_create(lt8522ex_class, NULL, lt8522exs[lt8522ex_cnt].dev, NULL, lt8522exs[lt8522ex_cnt].name))) {
            lt8522ex_log("create class device for lt8522ex input failed \n");
            //错误处理
            switch_dev_unregister(&lt8522exs[lt8522ex_cnt].sw_vga_out);
            kfree(lt8522exs[lt8522ex_cnt].sw_vga_out.name);
            switch_dev_unregister(&lt8522exs[lt8522ex_cnt].sw_hdmi_out);
            kfree(lt8522exs[lt8522ex_cnt].sw_hdmi_out.name);
            switch_dev_unregister(&lt8522exs[lt8522ex_cnt].sw_vga_in);
            kfree(lt8522exs[lt8522ex_cnt].sw_vga_in.name);
            switch_dev_unregister(&lt8522exs[lt8522ex_cnt].sw_hdmi_in);
            kfree(lt8522exs[lt8522ex_cnt].sw_hdmi_in.name);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr3);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr2);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr1);
            i2c_unregister_device(lt8522exs[lt8522ex_cnt].client_addr0);
            i2c_put_adapter(lt8522exs[lt8522ex_cnt].adapter);
            lt8522ex_free_gpio(&lt8522exs[lt8522ex_cnt]);
            continue;
        }

        lt8522ex_log("probe lt8522ex(%s) @i2c-%d \n", lt8522exs[lt8522ex_cnt].name, i2c_bus);
        lt8522ex_cnt += 1;
    }

    wake_up_process(poll_task);
    lt8522ex_log("probe %d lt8522ex in board \n", lt8522ex_cnt);

    return 0;

err4:
    class_destroy(lt8522ex_class);
err3:
    cdev_del(&lt8522ex_chrdev);
err2:
    unregister_chrdev_region(lt8522ex_dev_num, MAX_LT8522EX_CNT);
err1:
    return -1;
}

static int lt8522ex_remove(struct platform_device* pdev)
{
    int i;

    //退出轮询线程
    running = 0;
    kthread_stop(poll_task);

    //释放每片lt8522ex的私有资源
    for (i = 0; i < lt8522ex_cnt; i++) {
        device_destroy(lt8522ex_class, lt8522exs[i].dev);
        switch_dev_unregister(&lt8522exs[i].sw_vga_out);
        kfree(lt8522exs[lt8522ex_cnt].sw_vga_out.name);
        switch_dev_unregister(&lt8522exs[i].sw_hdmi_out);
        kfree(lt8522exs[lt8522ex_cnt].sw_hdmi_out.name);
        switch_dev_unregister(&lt8522exs[i].sw_vga_in);
        kfree(lt8522exs[lt8522ex_cnt].sw_vga_in.name);
        switch_dev_unregister(&lt8522exs[i].sw_hdmi_in);
        kfree(lt8522exs[lt8522ex_cnt].sw_hdmi_in.name);
        i2c_unregister_device(lt8522exs[i].client_addr3);
        i2c_unregister_device(lt8522exs[i].client_addr2);
        i2c_unregister_device(lt8522exs[i].client_addr1);
        i2c_unregister_device(lt8522exs[i].client_addr0);
        i2c_put_adapter(lt8522exs[i].adapter);
        lt8522ex_free_gpio(&lt8522exs[i]);
    }

    //释放公共资源
    class_destroy(lt8522ex_class);
    cdev_del(&lt8522ex_chrdev);
    unregister_chrdev_region(lt8522ex_dev_num, MAX_LT8522EX_CNT);

    return 0;
}

static struct of_device_id lt8522ex_ids[] = {
    {.compatible = "lt8522ex"},
    {}
};

static struct platform_driver lt8522ex_drv = {
    .driver = {
        .name = "lt8522ex",
        .of_match_table = of_match_ptr(lt8522ex_ids)
    },
    .probe = lt8522ex_probe,
    .remove = lt8522ex_remove
};

static int lt8522ex_init(void)
{
    lt8522ex_log("start do lt8522ex_init \n");
    return platform_driver_register(&lt8522ex_drv);
}

static void lt8522ex_exit(void)
{
    platform_driver_unregister(&lt8522ex_drv);
}

module_init(lt8522ex_init);
module_exit(lt8522ex_exit);

MODULE_AUTHOR("WUJING");
MODULE_DESCRIPTION("LT8522EX DRIVER");
MODULE_LICENSE("GPL");

