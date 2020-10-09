#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/netlink.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/gpio.h>
#include <linux/switch.h>
#include <linux/platform_device.h>

#include <net/netlink.h>
#include <net/net_namespace.h>
#include <net/sock.h>
#include <asm/io.h>

#include "lt6911uxc.h"
#include "sn_netlink.h"

static dev_t lt6911uxc_dev_num;         //lt6911uxc字符设备设备号起始值
static struct cdev lt6911uxc_chrdev;    //给用户空间提供的字符设备
static struct class* lt6911uxc_class;   //
static int lt6911uxc_cnt = 0;           //这块板子上所有的lt6911uxc的数量
static lt6911uxc_t lt6911uxcs[MAX_LT6911UXC_CNT];   //用于存放板子上所有的lt6911uxc
static struct i2c_board_info __initdata i2c_info[MAX_LT6911UXC_CNT] = {};

#if DETECT_POLICY == USE_INTERRUPT
//定义GPIO中断下半部处理任务
static struct work_struct gpio_interrupt_tasklet;   //用于向应用上报事件的中断下半部处理程序
//用于标记当前有哪些lt6911uxc的状态改变了需要被读取的
static unsigned int lt6911uxc_status_flag = 0;

#elif DETECT_POLICY == USE_POLL
static int thread_run = 1;                      //控制线程的退出
static struct task_struct* poll_task = NULL;    //用于执行轮询任务的线程
#endif

//文件操作函数申明
static int lt6911uxc_open(struct inode* inode, struct file* file);
static int lt6911uxc_release(struct inode* inode, struct file* file);
static long lt6911uxc_ioctl(struct file* file, unsigned int cmd, unsigned long args);

#if DETECT_POLICY == USE_INTERRUPT
//中断处理函数
irqreturn_t gpio_interrupt_handler(int irq, void* dev_id);
//中断下半部处理函数
void gpio_interrupt_tasklet_func(struct work_struct* work);
#endif

//更新lt6911uxc状态的函数
static int lt6911uxc_status_update(lt6911uxc_t* lt6911uxc);
//lt6911uxc I2C操作函数
static void lt6911uxc_i2c_enable(lt6911uxc_t* lt6911uxc);
static void lt6911uxc_i2c_disable(lt6911uxc_t* lt6911uxc);
static void lt6911uxc_i2c_set_bank(lt6911uxc_t* lt6911uxc, unsigned char bank);
static int lt6911uxc_i2c_read_byte(lt6911uxc_t* lt6911uxc, unsigned char addr, unsigned char* val);
static int lt6911uxc_i2c_write_byte(lt6911uxc_t* lt6911uxc, unsigned char addr, unsigned char val);

//lt6911uxc复位函数
static void lt6911uxc_reset(lt6911uxc_t* lt6911uxc);
//lt6911uxc静音函数, 关闭lt6911uxc的i2s输出
static void lt6911uxc_mute(lt6911uxc_t* lt6911uxc);
//lt6911uxc取消静音函数, 开启lt6911uxc的i2s输出
static void lt6911uxc_unmute(lt6911uxc_t* lt6911uxc);

#if DETECT_POLICY == USE_POLL
//轮询状态线程使用的函数
static int lt6911uxc_detect_thread(void* data);
#endif

static struct file_operations lt6911uxc_fops = {
    .open = lt6911uxc_open,
    .release = lt6911uxc_release,
    .unlocked_ioctl = lt6911uxc_ioctl
};

static void lt6911uxc_reset(lt6911uxc_t* lt6911uxc)
{
    //初始化复位引脚为输出, 默认输出高电平
    gpio_request(lt6911uxc->reset_pin, NULL);
    gpio_direction_output(lt6911uxcs[lt6911uxc_cnt].reset_pin, 1);
    usleep_range(20000, 20000);

    gpio_direction_output(lt6911uxc->reset_pin, 0);
    usleep_range(100000, 110000);
    gpio_direction_output(lt6911uxc->reset_pin, 1);
    usleep_range(100000, 110000);

    gpio_free(lt6911uxc->reset_pin);
}

static void lt6911uxc_mute(lt6911uxc_t* lt6911uxc)
{
    unsigned char reg_b032;
    unsigned char reg_83b6;
    unsigned char reg_83b7;
    unsigned char reg_83ba;

    lt6911uxc_i2c_enable(lt6911uxc);

    lt6911uxc_i2c_set_bank(lt6911uxc, 0xb0);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x32, &reg_b032);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0x32, reg_b032 | (1 << 7));
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x32, &reg_b032);    //回读校验
    lt6911uxc_dbg("reg_b032:0x%x \n", reg_b032);

    lt6911uxc_i2c_set_bank(lt6911uxc, 0x83);
    //关闭DATA
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xba, &reg_83ba);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xba, reg_83ba & (~(0b111 << 4)));
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xba, &reg_83ba);
    lt6911uxc_dbg("reg_83ba:0x%x \n", reg_83ba);

    //关闭WS
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xb6, &reg_83b6);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xb6, reg_83b6 & (~(0b111 << 4)));
    //关闭SCK
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xb6, &reg_83b6);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xb6, reg_83b6 & (~(0b111)));
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xb6, &reg_83b6);
    lt6911uxc_dbg("reg_83b6:0x%x \n", reg_83b6);

    //关闭MCLK
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xb7, &reg_83b7);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xb7, reg_83b7 & (~(0b111 << 4)));
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xb7, &reg_83b7);
    lt6911uxc_dbg("reg_83b7:0x%x \n", reg_83b7);

    lt6911uxc_i2c_disable(lt6911uxc);
}

static void lt6911uxc_unmute(lt6911uxc_t* lt6911uxc)
{
    unsigned char reg_b032;
    unsigned char reg_83b6;
    unsigned char reg_83b7;
    unsigned char reg_83ba;

    lt6911uxc_i2c_enable(lt6911uxc);

    lt6911uxc_i2c_set_bank(lt6911uxc, 0xb0);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x32, &reg_b032);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0x32, reg_b032 & (~(1 << 7)));
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x32, &reg_b032);    //回读校验
    lt6911uxc_dbg("reg_b032:0x%x \n", reg_b032);

    lt6911uxc_i2c_set_bank(lt6911uxc, 0x83);
    //开启WS
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xb6, &reg_83b6);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xb6, reg_83b6 | (0b111 << 4));
    //开启SCK
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xb6, &reg_83b6);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xb6, reg_83b6 | 0b111);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xb6, &reg_83b6);
    lt6911uxc_dbg("reg_83b6:0x%x \n", reg_83b6);

    //开启MCLK
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xb7, &reg_83b7);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xb7, reg_83b7 | (0b111 << 4));
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xb7, &reg_83b7);
    lt6911uxc_dbg("reg_83b7:0x%x \n", reg_83b7);

    //开启DATA
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xba, &reg_83ba);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xba, reg_83ba | (0b111 << 4));
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xba, &reg_83ba);
    lt6911uxc_dbg("reg_83ba:0x%x \n", reg_83ba);

    lt6911uxc_i2c_disable(lt6911uxc);
}

static int lt6911uxc_open(struct inode* inode, struct file* file)
{
    if (MINOR(inode->i_rdev) >= lt6911uxc_cnt) {
        lt6911uxc_log("invalid minor number \n");
        return -1;
    }

    file->private_data = &lt6911uxcs[MINOR(inode->i_rdev)];
    lt6911uxc_log("do %s open \n", lt6911uxcs[MINOR(inode->i_rdev)].name);

    return 0;
}

static int lt6911uxc_release(struct inode* inode, struct file* file)
{
    lt6911uxc_t* lt6911uxc = (lt6911uxc_t*)file->private_data;
    lt6911uxc_log("%s close \n", lt6911uxc->name);

    return 0;
}

static long lt6911uxc_ioctl(struct file* file, unsigned int cmd, unsigned long args)
{
    lt6911uxc_t* lt6911uxc = (lt6911uxc_t*)file->private_data;

    switch (cmd) {
        case CMD_LT6911UXC_STATUS_GET:
            return copy_to_user((void*)args, &lt6911uxc->hdmi_event, sizeof(hdmi_event_t));
        case CMD_LT6911UXC_RESET:
            lt6911uxc_reset(lt6911uxc);
            break;
        case CMD_LT6911UXC_MUTE:
            lt6911uxc_mute(lt6911uxc);
            break;
        case CMD_LT6911UXC_UNMUTE:
            lt6911uxc_unmute(lt6911uxc);
            break;

        default:
            lt6911uxc_log("unsupport command called \n");
            break;
    }

    return 0;
}

#if DETECT_POLICY == USE_INTERRUPT
irqreturn_t gpio_interrupt_handler(int irq, void* dev_id)
{
    int index;  //用于判定哪片lt6911uxc发生了事件
    lt6911uxc_t* lt6911uxc = (lt6911uxc_t*)dev_id;

    //之所以不用判断哪根引脚发生了中断，是因为Linux新的GPIO框架为每个GPIO引脚映射了一个唯一的中断号
    index = ((void*)lt6911uxc - (void*)lt6911uxcs) / sizeof(lt6911uxc_t);
    lt6911uxc_status_flag |= (1 << index);  //标记对应的lt6911uxc有事件发生

    //调用中断下半部用于执行状态获取、上报应用操作
    schedule_work(&gpio_interrupt_tasklet);

    return IRQ_HANDLED;
}

void gpio_interrupt_tasklet_func(struct work_struct* work)
{
    int i;

    lt6911uxc_dbg("interrupt tasklet called \n");

    for (i = 0; i < lt6911uxc_cnt; i++) {
        if (lt6911uxc_status_flag & (1 << i)) {
            lt6911uxc_status_update(&lt6911uxcs[i]);    //读取相关寄存器, 更新状态
            lt6911uxc_status_flag &= (~(1 << i));       //清除标志
        }
    }
}
#elif DETECT_POLICY == USE_POLL
static int lt6911uxc_detect_thread(void* data)
{
    int i;
    unsigned int curr_lt6911_state[lt6911uxc_cnt];
    unsigned int prev_lt6911_state[lt6911uxc_cnt];

    memset(curr_lt6911_state, 0, sizeof(curr_lt6911_state));
    memset(prev_lt6911_state, 0, sizeof(prev_lt6911_state));

    lt6911uxc_dbg("start lt6911uxc polling thread... \n");
    while (thread_run) {
        for (i = 0; i < lt6911uxc_cnt; i++) {
            curr_lt6911_state[i] = gpio_get_value(lt6911uxcs[i].interrupt_pin);
        }

        for (i = 0; i < lt6911uxc_cnt; i++) {
            if (prev_lt6911_state[i] == 1 && curr_lt6911_state[i] == 0) {
                lt6911uxc_dbg("%s: state change \n", lt6911uxcs[i].name);
                lt6911uxc_status_update(&lt6911uxcs[i]);
            }
        }

        memcpy(prev_lt6911_state, curr_lt6911_state, sizeof(prev_lt6911_state));

        usleep_range(50000, 50100);
    }

    return 0;
}
#endif

//操作lt6911uxc前需调用此函数使能lt6911uxc的i2c操作
static void lt6911uxc_i2c_enable(lt6911uxc_t* lt6911uxc)
{
    lt6911uxc_i2c_set_bank(lt6911uxc, 0x80);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xee, 0x01);
}

//操作lt6911uxc后需调用此函数禁用lt6911uxc的i2c操作
static void lt6911uxc_i2c_disable(lt6911uxc_t* lt6911uxc)
{
    lt6911uxc_i2c_set_bank(lt6911uxc, 0x80);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xee, 0x00);
}

//切换操作的bank
static void lt6911uxc_i2c_set_bank(lt6911uxc_t* lt6911uxc, unsigned char bank)
{
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xff, bank);
}

//读取一个字节的数据
static int lt6911uxc_i2c_read_byte(lt6911uxc_t* lt6911uxc, unsigned char addr, unsigned char* val)
{
    int ret;

    ret = i2c_smbus_read_byte_data(lt6911uxc->client, addr);
    if (ret < 0) {
        lt6911uxc_log("%s: do i2c read failed. addr:0x%.2x \n", lt6911uxc->name, addr);
        *val = -1;
        return ret;
    }

    *val = (unsigned char)(ret & 0xff);

    return 0;
}

//写一个字节的数据
static int lt6911uxc_i2c_write_byte(lt6911uxc_t* lt6911uxc, unsigned char addr, unsigned char val)
{
    int ret;

    ret = i2c_smbus_write_byte_data(lt6911uxc->client, addr, val);
    if (ret < 0) {
        lt6911uxc_log("%s: do i2c write failed. addr:0x%.2x  val:0x%.2x \n", lt6911uxc->name, addr, val);
    }

    return ret;
}

static int lt6911uxc_status_update(lt6911uxc_t* lt6911uxc)
{
    unsigned char val;                  //用于临时存寄存器中读取的变量
    unsigned int tmds_clk = 0;          //tmds时钟, 用于计算像素时钟
    unsigned int pixel_clk = 0;         //像素时钟, 用于计算帧率
    unsigned char is_hdmi_2_0 = 0;      //用于判断tmds clock与pixel clock之间的关系

    unsigned short htotal = 0;          //水平方向总像素
    unsigned short vtotal = 0;          //竖直方向总像素

    unsigned short hactive = 0;         //水平方向有效像素
    unsigned short vactive = 0;         //竖直方向有效像素

    unsigned int have_audio = 0;        //表示是否有音频输入
    unsigned short sample_rate = 0;     //输入音频采样率

    char nl_msg[128];                   //上报给应用层的netlink消息

    //清除事件标志
    lt6911uxc->hdmi_event.event_flag = 0;

    lt6911uxc_i2c_enable(lt6911uxc);

    //获取HDMI拔插状态
    lt6911uxc_i2c_set_bank(lt6911uxc, 0x86);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xa3, &val);

    lt6911uxc_dbg("%s: reg[0x%.4x]:0x%.x \n", lt6911uxc->name, (0x86 << 8) | 0xa3, val);
    if (val == 0x55) {
        lt6911uxc->hdmi_event.event_flag |= EVENT_HDMI_PLUG_IN;
    } else {
        lt6911uxc->hdmi_event.event_flag |= EVENT_HDMI_PLUG_OUT;
        lt6911uxc_i2c_set_bank(lt6911uxc, 0x86);
        lt6911uxc_i2c_write_byte(lt6911uxc, 0xa6, 0);
        lt6911uxc_i2c_disable(lt6911uxc);
        switch_set_state(&lt6911uxc->sw, 0);
        sprintf(nl_msg, LT6911UXC_NL_MSG_FMT, lt6911uxc->name, LT6911UXC_PLUG_OUT_STR);
        post_nl_msg(nl_msg, STAR_NET_DRIVER_MSG_GRP);   //上报netlink消息
        return 0;
    }

    //读取tmds clock
    lt6911uxc_i2c_set_bank(lt6911uxc, 0x87);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x50, &val);     //0x8750[3:0]
    lt6911uxc_dbg("%s: tmds clock %s \n", lt6911uxc->name, val & (1 << 4) ? "stable" : "unstable");
    tmds_clk |= ((val & 0x0f) << 16);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x51, &val);     //0x8751[7:0]
    tmds_clk |= (val << 8);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x52, &val);     //0x8752[7:0]
    tmds_clk |= val;

    //判断是否是HDMI2.0输入
    lt6911uxc_i2c_set_bank(lt6911uxc, 0xb0);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0xa2, &val);
    is_hdmi_2_0 = val & (1 << 0) ? !0 : !!0;

    //计算像素时钟
    pixel_clk = is_hdmi_2_0 ? 4 * tmds_clk: tmds_clk;

    //获取水平方向总像素和竖直方向总像素
    lt6911uxc_i2c_set_bank(lt6911uxc, 0x86);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x7a, &val);
    vtotal |= (val << 8);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x7b, &val);
    vtotal |= val;
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x7c, &val);
    htotal |= (val << 8);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x7d, &val);
    htotal |= val;

    htotal *= 2;

    //获取水平方向有效像素和竖直方向有效像素
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x7e, &val);
    vactive |= (val << 8);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x7f, &val);
    vactive |= val;
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x80, &val);
    hactive |= (val << 8);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x81, &val);
    hactive |= val;

    hactive *= 2;

    //采集音频相关信息
    lt6911uxc_i2c_set_bank(lt6911uxc, 0xb0);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x81, &val);
    have_audio = val & (1 << 5) ? !0 : !!0;
    if (have_audio) {
        lt6911uxc_i2c_read_byte(lt6911uxc, 0xaa, &val);
        sample_rate |= (val << 8);
        lt6911uxc_i2c_read_byte(lt6911uxc, 0xab, &val);
        sample_rate |= val;
    }

    //开启MIPI输出
    lt6911uxc_i2c_set_bank(lt6911uxc, 0x81);
    lt6911uxc_i2c_read_byte(lt6911uxc, 0x1d, &val);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0x1d, 0xfb);

    lt6911uxc_dbg("%s: reg[0x%.4x]:0x%.x \n", lt6911uxc->name, (0x81 << 8) | 0x1d, val);

    //清零0x86a6, 用于通知lt6911uxc事件已经被处理
    lt6911uxc_i2c_set_bank(lt6911uxc, 0x86);
    lt6911uxc_i2c_write_byte(lt6911uxc, 0xa6, 0);

    lt6911uxc_i2c_disable(lt6911uxc);

    if (lt6911uxc->hdmi_event.event_flag & EVENT_HDMI_PLUG_IN) {
        lt6911uxc_dbg("%s: resolution(%d, %d) frame_rate:%d \n", lt6911uxc->name, hactive, vactive, pixel_clk * 1000 / (htotal * vtotal));
        if (have_audio) {
            lt6911uxc_dbg("%s: audio sample rate:0x%x \n", lt6911uxc->name, sample_rate);
            lt6911uxc->hdmi_event.have_audio = 1;
            lt6911uxc->hdmi_event.samplerate = sample_rate;
        }
    }

    if (pixel_clk > 0 && pixel_clk < 80000) {
        lt6911uxc->hdmi_event.lane_count = 1;
    } else if(pixel_clk >= 80000 && pixel_clk < 150000) {
        lt6911uxc->hdmi_event.lane_count = 2;
    } else if(pixel_clk >= 150000 && pixel_clk <= 340000) {
        lt6911uxc->hdmi_event.lane_count = 4;
    } else if(pixel_clk > 340000) {
        lt6911uxc->hdmi_event.lane_count = 8;
    } else {
        lt6911uxc_log("unsupport mipi csi lane count \n");
        //标记有未知错误发生, 应用程序在检测到这个标志时应妥善处理, 避免显示异常
        lt6911uxc->hdmi_event.event_flag |= EVENT_HDMI_ERROR;
    }

    lt6911uxc_dbg("mipi csi lane count: %d \n", lt6911uxc->hdmi_event.lane_count);

    lt6911uxc->hdmi_event.hactive = hactive;
    lt6911uxc->hdmi_event.vactive = vactive;
    lt6911uxc->hdmi_event.framerate = pixel_clk * 1000 / (htotal * vtotal);

    switch_set_state(&lt6911uxc->sw, 1);
    sprintf(nl_msg, LT6911UXC_NL_MSG_FMT, lt6911uxc->name, LT6911UXC_PLUG_IN_STR);
    post_nl_msg(nl_msg, STAR_NET_DRIVER_MSG_GRP);

    return 0;
}

static int lt6911uxc_probe(struct platform_device* pdev)
{
    int ret;
    unsigned int i2c_bus;               //lt6911uxc所接的总线号
    unsigned int dev_addr;              //lt6911uxc的设备地址
    struct device_node* child;          //用于遍历所有lt6911uxc节点的指针

    int idx;
    unsigned int val;
    unsigned int reg_addr;
    volatile unsigned int* reg_ptr;

    lt6911uxc_dbg("start probe lt6911uxc in board \n");
    if (pdev->dev.of_node == NULL) {
        lt6911uxc_log("not device tree node associated with this device \n");
        return -1;
    }

    //动态分配字符设备号
    ret = alloc_chrdev_region(&lt6911uxc_dev_num, 0, MAX_LT6911UXC_CNT, "lt6911uxc_region");
    if (ret != 0) {
        lt6911uxc_log("alloc character device number failed \n");
        goto err1;
    }

    //初始化字符设备
    cdev_init(&lt6911uxc_chrdev, &lt6911uxc_fops);
    ret = cdev_add(&lt6911uxc_chrdev, lt6911uxc_dev_num, MAX_LT6911UXC_CNT);
    if (ret !=0 ){
        lt6911uxc_log("register character device to kernel failed \n");
        goto err2;
    }

    //创建用户空间的字符设备节点
    lt6911uxc_class = class_create(THIS_MODULE, "lt6911uxc_class");
    if (IS_ERR(lt6911uxc_class)) {
        lt6911uxc_log("create lt6911uxc_class failed \n");
        goto err3;
    }

    //创建中断下半部任务或是创建内核线程
#if DETECT_POLICY == USE_INTERRUPT
    INIT_WORK(&gpio_interrupt_tasklet, gpio_interrupt_tasklet_func);
#elif DETECT_POLICY == USE_POLL
    poll_task = kthread_create(lt6911uxc_detect_thread, NULL, "lt6911uxc polling thread");
    if (poll_task == NULL) {
        lt6911uxc_log("create lt6911uxc polling thread failed \n");
        goto err4;
    }
#endif

    //遍历子节点, 读取每片lt6911uxc的属性
    for_each_child_of_node(pdev->dev.of_node, child) {
        lt6911uxc_dbg("parse device node[%s] \n", child->full_name);

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
                lt6911uxc_dbg("set reg[0x%x] to val[0x%x] \n", reg_addr, val);
            }
        }

        //获取lt6911uxc的名字
        ret = of_property_read_string(child, "devname", (const char**)(&lt6911uxcs[lt6911uxc_cnt].name));
        if (ret != 0) {
            lt6911uxc_log("get property[%s] failed \n", "devname");
            continue;
        }

        //获取lt6911uxc的总线地址
        ret = of_property_read_u32_index(child, "i2c_info", 0, &i2c_bus);
        if (ret != 0) {
            lt6911uxc_log("get number of i2c bus failed \n");
            continue;
        }

        //获取lt6911uxc设备地址
        ret = of_property_read_u32_index(child, "i2c_info", 1, &dev_addr);
        if (ret != 0) {
            lt6911uxc_log("get device address of lt6911uxc failed \n");
            continue;
        }

        //获取lt6911uxc所接的复位引脚(若无复位引脚, 设备树中可不配置)
        ret = of_property_read_u32(child, "reset_pin", &lt6911uxcs[lt6911uxc_cnt].reset_pin);
        if (ret != 0) {
            lt6911uxc_log("get reset pin of lt6911uxc failed \n");
            lt6911uxcs[lt6911uxc_cnt].reset_pin = -1;
        }
        lt6911uxc_dbg("reset_pin of %s:GPIO%d_%d \n", lt6911uxcs[lt6911uxc_cnt].name, lt6911uxcs[lt6911uxc_cnt].reset_pin / 8, lt6911uxcs[lt6911uxc_cnt].reset_pin % 8);

        //获取lt6911uxc所接的中断引脚
        ret = of_property_read_u32(child, "inter_pin", &lt6911uxcs[lt6911uxc_cnt].interrupt_pin);
        if (ret != 0) {
            lt6911uxc_log("get interrupt pin of lt6911uxc failed \n");
            continue;
        }
        lt6911uxc_dbg("inter_pin of %s:GPIO%d_%d \n", lt6911uxcs[lt6911uxc_cnt].name, lt6911uxcs[lt6911uxc_cnt].interrupt_pin / 8, lt6911uxcs[lt6911uxc_cnt].interrupt_pin % 8);

        //初始化中断引脚为输入
        gpio_request(lt6911uxcs[lt6911uxc_cnt].interrupt_pin, NULL);
        gpio_direction_input(lt6911uxcs[lt6911uxc_cnt].interrupt_pin);

        if (lt6911uxcs[lt6911uxc_cnt].reset_pin >= 0) {
            //复位lt6911uxc
            lt6911uxc_reset(&lt6911uxcs[lt6911uxc_cnt]);
        }

        //初始化lt6911uxc的字符设备号(这样做的目的是后续可以通过从设备号获取到对应的lt6911uxc对象)
        lt6911uxcs[lt6911uxc_cnt].dev = MKDEV(MAJOR(lt6911uxc_dev_num), lt6911uxc_cnt);
#if DETECT_POLICY == USE_INTERRUPT
        lt6911uxcs[lt6911uxc_cnt].irq = gpio_to_irq(lt6911uxcs[lt6911uxc_cnt].interrupt_pin);
#endif
        lt6911uxcs[lt6911uxc_cnt].sw.name = lt6911uxcs[lt6911uxc_cnt].name;

        //申请i2c资源
        memset(i2c_info[lt6911uxc_cnt].type, 0, I2C_NAME_SIZE);
        strncpy(i2c_info[lt6911uxc_cnt].type, lt6911uxcs[lt6911uxc_cnt].name, I2C_NAME_SIZE - 1);
        i2c_info[lt6911uxc_cnt].addr = dev_addr >> 1;

        lt6911uxcs[lt6911uxc_cnt].adapter = i2c_get_adapter(i2c_bus);
        if (lt6911uxcs[lt6911uxc_cnt].adapter == NULL) {
            lt6911uxc_log("get i2c-%d's adapter failed \n");
            continue;
        }

        lt6911uxcs[lt6911uxc_cnt].client = i2c_new_device(lt6911uxcs[lt6911uxc_cnt].adapter, &i2c_info[lt6911uxc_cnt]);
        if (lt6911uxcs[lt6911uxc_cnt].client == NULL) {
            i2c_put_adapter(lt6911uxcs[lt6911uxc_cnt].adapter);
            continue;
        }

        //创建字符设备
        if (IS_ERR(device_create(lt6911uxc_class, NULL, lt6911uxcs[lt6911uxc_cnt].dev, NULL, lt6911uxcs[lt6911uxc_cnt].name))) {
            lt6911uxc_log("create char device for %s failed \n", lt6911uxcs[lt6911uxc_cnt].name);
            i2c_unregister_device(lt6911uxcs[lt6911uxc_cnt].client);
            i2c_put_adapter(lt6911uxcs[lt6911uxc_cnt].adapter);
            continue;
        }

        //注册switch设备
        ret = switch_dev_register(&lt6911uxcs[lt6911uxc_cnt].sw);
        if (ret != 0) {
            lt6911uxc_log("register switch device for %s failed \n", lt6911uxcs[lt6911uxc_cnt].name);
            device_destroy(lt6911uxc_class, lt6911uxcs[lt6911uxc_cnt].dev);
            i2c_unregister_device(lt6911uxcs[lt6911uxc_cnt].client);
            i2c_put_adapter(lt6911uxcs[lt6911uxc_cnt].adapter);
            continue;
        }

        //更新状态
        lt6911uxc_status_update(&lt6911uxcs[lt6911uxc_cnt]);

#if DETECT_POLICY == USE_INTERRUPT
        //注册中断处理函数
        ret = request_irq(lt6911uxcs[lt6911uxc_cnt].irq, gpio_interrupt_handler, IRQF_SHARED | IRQF_TRIGGER_FALLING, lt6911uxcs[lt6911uxc_cnt].name, &lt6911uxcs[lt6911uxc_cnt]);
#endif

        lt6911uxc_log("probe lt6911uxc(%s)@i2c-%d with device address 0x%x \n", lt6911uxcs[lt6911uxc_cnt].name, i2c_bus, dev_addr);
        lt6911uxc_cnt += 1;

        if (lt6911uxc_cnt >= MAX_LT6911UXC_CNT) {
            lt6911uxc_log("reach max support count of lt6911uxc \n");
            break;
        }
    }

#if DETECT_POLICY == USE_POLL
    //启动轮询线程
    wake_up_process(poll_task);
#endif

    lt6911uxc_log("probe %d lt6911uxcs in board \n", lt6911uxc_cnt);

    return 0;

#if DETECT_POLICY == USE_POLL
err4:
#endif
    class_destroy(lt6911uxc_class);
err3:
    cdev_del(&lt6911uxc_chrdev);
err2:
    //释放占有的设备号
    unregister_chrdev_region(lt6911uxc_dev_num, MAX_LT6911UXC_CNT);
err1:
    return -1;
}

static int lt6911uxc_remove(struct platform_device* pdev)
{
    int i;

    //第一步, 停止线程或解注册中断
#if DETECT_POLICY == USE_POLL
    thread_run = 0;
    kthread_stop(poll_task);
#elif DETECT_POLICY == USE_INTERRUPT
    for (i = 0; i < lt6911uxc_cnt; i++) {
        free_irq(lt6911uxcs[i].irq, &lt6911uxcs[i]);
    }
#endif

    //释放每片lt6911uxc的私有资源
    for (i = 0; i < lt6911uxc_cnt; i++) {
        switch_dev_unregister(&lt6911uxcs[i].sw);
        i2c_unregister_device(lt6911uxcs[i].client);
        i2c_put_adapter(lt6911uxcs[i].adapter);
        device_destroy(lt6911uxc_class, lt6911uxcs[i].dev);
        gpio_free(lt6911uxcs[i].interrupt_pin);
    }

    //释放驱动的公共资源
    class_destroy(lt6911uxc_class);
    cdev_del(&lt6911uxc_chrdev);
    unregister_chrdev_region(lt6911uxc_dev_num, MAX_LT6911UXC_CNT);

    return 0;
}

static struct of_device_id lt6911uxc_ids[] = {
    {.compatible = "lt6911uxc"},
    {}
};

static struct platform_driver lt6911uxc_drv = {
    .driver = {
        .name = "lt6911uxc",
        .of_match_table = of_match_ptr(lt6911uxc_ids)
    },
    .probe = lt6911uxc_probe,
    .remove = lt6911uxc_remove
};

static int lt6911uxc_init(void)
{
    return platform_driver_register(&lt6911uxc_drv);
}

static void lt6911uxc_exit(void)
{
    platform_driver_unregister(&lt6911uxc_drv);
}

module_init(lt6911uxc_init);
module_exit(lt6911uxc_exit);

MODULE_AUTHOR("WUJING");
MODULE_DESCRIPTION("LT6911UXC STATUS DETECT DRIVER");
MODULE_LICENSE("GPL");
