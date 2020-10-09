#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/syslog.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/netlink.h>
#include <linux/gpio.h>
#include <linux/proc_fs.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "sn_netlink.h"
#include "fpga_monitor.h"
#include "fpga_flash.h"

static int fpga_read_byte(unsigned char addr, unsigned char* val);
static int fpga_write_byte(unsigned char addr, unsigned char val);

static int fpga_monitor_probe(struct platform_device* pdev);
static int fpga_monitor_remove(struct platform_device* pdev);

static int fpga_show_statistic(void);
static int fpga_statistic_init(void);
static int fpga_statistic_update(void);

static void fpga_interrupt_tasklet_func(struct work_struct* work);

static int fpga_fops_open(struct inode* inode, struct file* file);
static int fpga_fops_release(struct inode* inode, struct file* file);
static ssize_t fpga_fops_read(struct file* file, char __user* buffer, size_t length, loff_t* offset);
static ssize_t fpga_fops_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset);
static long fpga_fops_ioctl(struct file* file, unsigned int cmd, unsigned long args);

static void sdi_y_abnormal_process(void);
static void sdi_c_abnormal_process(void);
static int vga_hotplug_process(void);
static int sdi_hotplug_process(void);
static int poe_process(void);

static int fpga_spi_flash_probe(struct spi_device *spi);
static int fpga_spi_flash_remove(struct spi_device *spi);

static void fpga_print_usage(void);

fpga_monitor_t* fpga_monitor = NULL;
static fpga_statistic_t* fpga_statistic = NULL;
static struct i2c_board_info i2c_info;
static struct work_struct fpga_interrupt_tasklet;

static char* proc_msg_buf = NULL;   //用于存放转换成字符串后的fpga_statistic信息
static unsigned int index = 0;      //用于遍历proc_msg_buf

static struct of_device_id spi_flash_id = {
    .compatible = "spi-flash"
};

static struct file_operations fpga_fops = {
    .open = fpga_fops_open,
    .release = fpga_fops_release,
    .read = fpga_fops_read,
    .write = fpga_fops_write,
    .unlocked_ioctl = fpga_fops_ioctl
};

static struct spi_driver spi_flash_driver = {
    .driver = {
        .name = "spi-flash",
        .of_match_table = &spi_flash_id
    },
    .probe = fpga_spi_flash_probe,
    .remove = fpga_spi_flash_remove,
};

static int fpga_read_byte(unsigned char addr, unsigned char* val)
{
    int ret;

    ret = i2c_smbus_read_byte_data(fpga_monitor->client, addr);
    if (ret >= 0) {
        *val = ret & 0xff;
    }

    return ret < 0 ? -1 : 0;
}

static int fpga_write_byte(unsigned char addr, unsigned char val)
{
    int ret;

    ret = i2c_smbus_write_byte_data(fpga_monitor->client, addr, val);
    if (ret < 0) {
        fpga_log("do i2c write failed. addr:0x%.2x  val:0x%.2x \n",addr, val);
    }

    return ret;
}

static int fpga_spi_flash_probe(struct spi_device *spi)
{
    fpga_log("\n");

    fpga_monitor->spi_dev = spi;

    return 0;
}

static int fpga_spi_flash_remove(struct spi_device *spi)
{
    fpga_monitor->spi_dev = NULL;

    return 0;
}

static int fpga_show_statistic(void)
{
    unsigned char val;
    unsigned char led_pattern;

    fpga_statistic_update();

    //将fpga统计信息转换成字符串写入proc_msg_buf
    sprintf(proc_msg_buf, "\n=========================== FPGA STATUS ===========================\n");
    index = strlen(proc_msg_buf);
    sprintf(proc_msg_buf + index, "FPGA VERSION:%d \n\n", fpga_statistic->version);
    index = strlen(proc_msg_buf);
    sprintf(proc_msg_buf + index, "FPGA INTERRUPT COUNT:%d \n", fpga_statistic->interr_cnt);
    index = strlen(proc_msg_buf);
    sprintf(proc_msg_buf + index, "FPGA SDI Y ABNORMAL COUNT:%d \n", fpga_statistic->sdi_y_abnor_cnt);
    index = strlen(proc_msg_buf);
    sprintf(proc_msg_buf + index, "FPGA SDI C ABNORMAL COUNT:%d \n\n", fpga_statistic->sdi_c_abnor_cnt);
    index = strlen(proc_msg_buf);

    sprintf(proc_msg_buf + index, "FPGA VIDEO IN SWITCH: %s \n", fpga_statistic->video_switch == FPGA_SWITCH_SDI_IN ? "SDI_IN" : fpga_statistic->video_switch == FPGA_SWITCH_VGA_IN ? "VGA_IN" : "NONE");
    index = strlen(proc_msg_buf);

    //获得绿色LED显示样式
    led_pattern = ((FPGA_LED_PATTERN_MASK << FPGA_SYS_LED_PATTERN_SHIFT) & fpga_statistic->led_pattern) >> FPGA_SYS_LED_PATTERN_SHIFT;
    sprintf(proc_msg_buf + index, "FPGA LED PATTERN: GREEN LED(%s) \n", led_pattern == FPGA_LED_PATTERN_ON ? "ON" : led_pattern == FPGA_LED_PATTERN_OFF ? "OFF" : led_pattern == FPGA_LED_PATTERN_FLASH ? "flash" : "unknown");
    index = strlen(proc_msg_buf);
    //获取红色LED显示样式
    led_pattern = ((FPGA_LED_PATTERN_MASK << FPGA_SLEEP_LED_PATTERN_SHIFT) & fpga_statistic->led_pattern) >> FPGA_SLEEP_LED_PATTERN_SHIFT;
    sprintf(proc_msg_buf + index, "                  RED LED(%s) \n\n", led_pattern == FPGA_LED_PATTERN_ON ? "ON" : led_pattern == FPGA_LED_PATTERN_OFF ? "OFF" : led_pattern == FPGA_LED_PATTERN_FLASH ? "flash" : "unknown");
    index = strlen(proc_msg_buf);

    sprintf(proc_msg_buf + index, "VGA INFO: \n");
    index = strlen(proc_msg_buf);
    sprintf(proc_msg_buf + index, "    STATUS:%s \n", fpga_statistic->vga_info.in ? "PLUG_IN" : "PLUG_OUT");
    index = strlen(proc_msg_buf);
    sprintf(proc_msg_buf + index, "    RESOLUTION:%d*%d \n", fpga_statistic->vga_info.resolution_width, fpga_statistic->vga_info.resolution_height);
    index = strlen(proc_msg_buf);
    sprintf(proc_msg_buf + index, "    FRAME RATE:%dfps \n\n", fpga_statistic->vga_info.frame_rate);
    index = strlen(proc_msg_buf);

    sprintf(proc_msg_buf + index, "SDI INFO: \n");
    index = strlen(proc_msg_buf);
    sprintf(proc_msg_buf + index, "    STATUS:%s \n", fpga_statistic->sdi_info.in ? "PLUG_IN" : "PLUG_OUT");
    index = strlen(proc_msg_buf);
    sprintf(proc_msg_buf + index, "    RESOLUTION:%d*%d \n", fpga_statistic->sdi_info.resolution_width, fpga_statistic->sdi_info.resolution_height);
    index = strlen(proc_msg_buf);
    sprintf(proc_msg_buf + index, "    FRAME RATE:%dfps \n\n", fpga_statistic->sdi_info.frame_rate);
    index = strlen(proc_msg_buf);

    sprintf(proc_msg_buf + index, "LANE COUNT: %d \n\n", fpga_statistic->lane_count);
    index = strlen(proc_msg_buf);

    sprintf(proc_msg_buf + index, "FPGA REGISTER DUMP: \n");
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_VERSION_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_VERSION_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_SDI_FRAME_RATE_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_SDI_FRAME_RATE_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_VGA_FRAME_RATE_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_VGA_FRAME_RATE_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_VIDEO_IN_DETECT_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_VIDEO_IN_DETECT_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_VIDEO_IN_SWITCH_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_VIDEO_IN_SWITCH_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_INTER_STATUS_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_INTER_STATUS_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_SDI_WIDTH_H_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_SDI_WIDTH_H_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_SDI_WIDTH_L_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_SDI_WIDTH_L_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_SDI_HEIGHT_H_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_SDI_HEIGHT_H_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_SDI_HEIGHT_L_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_SDI_HEIGHT_L_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_VGA_WIDTH_H_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_VGA_WIDTH_H_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_VGA_WIDTH_L_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_VGA_WIDTH_L_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_VGA_HEIGHT_H_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_VGA_HEIGHT_H_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_VGA_HEIGHT_L_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_VGA_HEIGHT_L_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_LED_PATTERN_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_LED_PATTERN_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_INTER_CLEAR_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n", FPGA_INTER_CLEAR_REG, val);
    index = strlen(proc_msg_buf);
    fpga_read_byte(FPGA_LANE_COUNT_REG, &val);
    sprintf(proc_msg_buf + index, "    ADDR:0x%.2x  VALUE:0x%.2x \n\n", FPGA_LANE_COUNT_REG, val);
    index = strlen(proc_msg_buf);

    return 0;
}

static int fpga_statistic_init(void)
{
    int ret;

    fpga_statistic->led_pattern = 0;
    fpga_statistic->led_pattern |= (FPGA_LED_PATTERN_FLASH << FPGA_SYS_LED_PATTERN_SHIFT);  //绿灯闪烁
    fpga_statistic->video_switch = FPGA_SWITCH_SDI_IN;  //默认配置SDI输入

    ret = fpga_read_byte(FPGA_VERSION_REG, &fpga_statistic->version);
    if (ret != 0) {
        fpga_log("get fpga version failed \n");
        return -1;
    }

    fpga_write_byte(FPGA_LED_PATTERN_REG, fpga_statistic->led_pattern);
    fpga_write_byte(FPGA_VIDEO_IN_SWITCH_REG, fpga_statistic->video_switch);

    //初始化vga、sdi接口状态
    vga_hotplug_process();
    sdi_hotplug_process();

    //清除fpga中断状态寄存器
    fpga_write_byte(FPGA_INTER_CLEAR_REG, 0x00);

    return 0;
}

static int fpga_statistic_update(void)
{
    int ret;

    ret = fpga_read_byte(FPGA_VERSION_REG, &fpga_statistic->version);
    if (ret != 0) {
        fpga_log("get fpga version failed \n");
        return -1;
    }

    fpga_read_byte(FPGA_LED_PATTERN_REG, &fpga_statistic->led_pattern);
    fpga_read_byte(FPGA_VIDEO_IN_SWITCH_REG, &fpga_statistic->video_switch);
    fpga_read_byte(FPGA_LANE_COUNT_REG, &fpga_statistic->lane_count);

    vga_hotplug_process();
    sdi_hotplug_process();

    return 0;
}

static void fpga_print_usage(void)
{
    printk("\nUSAGE: \n");
    printk("    echo set_led {red | green} {on | off | flash} > /proc/fpga_monitor \n");
    printk("    echo set_vi {SDI | VGA} > /proc/fpga_monitor \n");
}

static int fpga_fops_open(struct inode* inode, struct file* file)
{
    proc_msg_buf = (char*)kzalloc(FPGA_PROC_MSG_BUFF_SIZE, GFP_KERNEL);
    if (proc_msg_buf == NULL) {
        fpga_log("alloc memory for fpga message buffer failed \n");
        return -1;
    }

    index = 0;  //index恢复到0, 准备开始写fpga统计信息
    fpga_show_statistic();
    index = 0;  //index恢复到0, 准备开始读fpga统计信息

    return 0;
}

static int fpga_fops_release(struct inode* inode, struct file* file)
{
    kfree(proc_msg_buf);

    return 0;
}

static ssize_t fpga_fops_read(struct file* file, char __user* buffer, size_t length, loff_t* offset)
{
    int ret;
    int copy_count = 0;     //要拷贝给应用的字节数
    size_t reserve_char = strlen(proc_msg_buf) - index + 1; //proc_msg_buf中未拷贝的字节数

    copy_count = min(length, reserve_char);
    ret = copy_to_user(buffer, proc_msg_buf + index, copy_count);
    index += (copy_count - ret);

    return copy_count - ret;
}

static ssize_t fpga_fops_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset)
{
    int ret;
    char cmd[32];
    char arg1[32];
    char arg2[32];
    unsigned char led_ctrl_shift;

    ret = sscanf(buffer, FPGA_PROC_CMD_FMT, cmd, arg1, arg2);
    if (ret > 3 || ret < 2) {
        fpga_print_usage();
    } else {
        if (strcmp(cmd, FPGA_PROC_CMD_SET_LED) == 0) {
            if (ret != 3) {
                fpga_print_usage();
                return length;
            }

            if (strcmp(arg1, "red") == 0) {
                led_ctrl_shift = FPGA_SLEEP_LED_PATTERN_SHIFT;
            } else if(strcmp(arg1, "green") == 0) {
                led_ctrl_shift = FPGA_SYS_LED_PATTERN_SHIFT;
            } else {
                fpga_print_usage();
                return length;
            }

            fpga_statistic->led_pattern &= ~(FPGA_LED_PATTERN_MASK << led_ctrl_shift);
            if (strcmp(arg2, "on") == 0) {
                fpga_statistic->led_pattern |= (FPGA_LED_PATTERN_ON << led_ctrl_shift);
            } else if(strcmp(arg2, "off") == 0) {
                fpga_statistic->led_pattern |= (FPGA_LED_PATTERN_OFF << led_ctrl_shift);
            } else if(strcmp(arg2, "flash") == 0) {
                fpga_statistic->led_pattern |= (FPGA_LED_PATTERN_FLASH << led_ctrl_shift);
            } else {
                fpga_print_usage();
                return length;
            }

            fpga_write_byte(FPGA_LED_PATTERN_REG, fpga_statistic->led_pattern);
        } else if(strcmp(cmd, FPGA_PROC_CMD_SET_VI) == 0) {
            if (strcmp(arg1, "SDI") == 0) {
                ret = fpga_write_byte(FPGA_VIDEO_IN_SWITCH_REG, FPGA_SWITCH_SDI_IN);
                if (ret == 0) {
                    //同步驱动中维护的视频输入源切换开关
                    fpga_statistic->video_switch = FPGA_SWITCH_SDI_IN;
                    fpga_statistic->event_bits |= FPGA_EVENT_SDI_HOTPLUG;
                    fpga_statistic->event_bits |= FPGA_VIDEO_IN_CHANGED;
                    /**
                     * 触发上报一次netlink事件, 这样用户在控制台echo set_vi xxx > /proc/fpga_monitor
                     * 后, 应用程序能收到netlink事件, 然后重新读取切换完后的视频输入接口的视频参数并配置应用
                     * */
                    post_nl_msg(FPGA_NET_LINK_MSG, STAR_NET_DRIVER_MSG_GRP);    //向应用上报消息
                }
            } else if(strcmp(arg1, "VGA") == 0) {
                ret = fpga_write_byte(FPGA_VIDEO_IN_SWITCH_REG, FPGA_SWITCH_VGA_IN);
                if (ret == 0) {
                    //同步驱动中维护的视频输入源切换开关
                    fpga_statistic->video_switch = FPGA_SWITCH_VGA_IN;
                    fpga_statistic->event_bits |= FPGA_EVENT_VGA_HOTPLUG;
                    fpga_statistic->event_bits |= FPGA_VIDEO_IN_CHANGED;
                    /**
                     * 触发上报一次netlink事件, 这样用户在控制台echo set_vi xxx > /proc/fpga_monitor
                     * 后, 应用程序能收到netlink事件, 然后重新读取切换完后的视频输入接口的视频参数并配置应用
                     * */
                    post_nl_msg(FPGA_NET_LINK_MSG, STAR_NET_DRIVER_MSG_GRP);    //向应用上报消息
                }
            } else {
                fpga_print_usage();
                return length;
            }
        } else {
            fpga_print_usage();
        }
    }

    return length;
}

static long fpga_fops_ioctl(struct file* file, unsigned int cmd, unsigned long args)
{
    int ret;
    update_package_t* pkg;

    switch (cmd) {
    case FPGA_IOCTL_CMD_GET_STATISTIC:
        //更新一些状态, 使fpga的状态和软件状态同步
        fpga_statistic_update();
        ret = copy_to_user((fpga_statistic_t*)args, fpga_statistic, sizeof(fpga_statistic_t));
        if (ret == 0) {
            fpga_statistic->event_bits = 0;             //用户读取后清除事件标志, 防止下回应用误认为有
        }
        return ret;
    case FPGA_IOCTL_CMD_SET_LED:
        ret = fpga_write_byte(FPGA_LED_PATTERN_REG, args & 0x0f);
        if (ret == 0) {
            fpga_statistic->led_pattern = args & 0x0f;  //更新LED显示样式维护信息
        }
        break;
    case FPGA_IOCTL_CMD_UPDATE:
        pkg = (update_package_t*)args;
        ret = fpga_flash_write(pkg->data, pkg->length);
        //触发fpga加载新的程序
        if (ret == 0) {
            gpio_direction_output(fpga_monitor->flash_ntf_pin, 1);
            usleep_range(10000, 10000);
            gpio_direction_output(fpga_monitor->flash_ntf_pin, 0);
            usleep_range(200000, 200000);
            gpio_direction_output(fpga_monitor->flash_ntf_pin, 1);
            usleep_range(10000, 10000);
            gpio_direction_input(fpga_monitor->flash_ntf_pin);      //操作完成后配置成高阻态
        }
        return ret;
    case FPGA_IOCTL_CMD_SET_VIDEO_SWITCH:
        ret = fpga_write_byte(FPGA_VIDEO_IN_SWITCH_REG, args & 0xff);
        if (ret == 0) {
            fpga_statistic->video_switch = args & 0xff; //更新维护信息
        }
        break;
    case FPGA_IOCTL_CMD_GET_BIN:
        pkg = (update_package_t*)args;
        return fpga_flash_read(pkg->data, pkg->length);
    default:
        fpga_log("unsupport command \n");
        return -1;
    }

    return 0;
}

//处理SDI Y分量异常
static void sdi_y_abnormal_process(void)
{
    fpga_statistic->sdi_y_abnor_cnt += 1;
    fpga_statistic->event_bits |= FPGA_EVENT_ABNORMAL;
}

//处理SDI c分量异常
static void sdi_c_abnormal_process(void)
{
    fpga_statistic->sdi_c_abnor_cnt += 1;
    fpga_statistic->event_bits |= FPGA_EVENT_ABNORMAL;
}

//处理vga热拔插中断
static int vga_hotplug_process(void)
{
    int ret = 0;
    unsigned char val;

    ret = fpga_read_byte(FPGA_VIDEO_IN_DETECT_REG, &val);
    if (ret != 0) {
        fpga_log("read reg[0x%x] failed \n", FPGA_VIDEO_IN_DETECT_REG);
        return -1;
    }

    fpga_statistic->vga_info.in = val & FPGA_VGA_IN_BIT ? 1 : 0;
    if (fpga_statistic->vga_info.in) {
        fpga_statistic->vga_info.resolution_width = 0;
        fpga_statistic->vga_info.resolution_height = 0;
        fpga_statistic->vga_info.frame_rate = 0;

        fpga_read_byte(FPGA_VGA_WIDTH_H_REG, &val);
        fpga_statistic->vga_info.resolution_width |= (val << 8);
        fpga_read_byte(FPGA_VGA_WIDTH_L_REG, &val);
        fpga_statistic->vga_info.resolution_width |= val;

        fpga_read_byte(FPGA_VGA_HEIGHT_H_REG, &val);
        fpga_statistic->vga_info.resolution_height |= (val << 8);
        fpga_read_byte(FPGA_VGA_HEIGHT_L_REG, &val);
        fpga_statistic->vga_info.resolution_height |= val;

        fpga_read_byte(FPGA_VGA_FRAME_RATE_REG, &val);
        fpga_statistic->vga_info.frame_rate = val;

        fpga_statistic->event_bits |= FPGA_EVENT_VGA_HOTPLUG;
    } else {
        fpga_statistic->vga_info.resolution_width = 0;
        fpga_statistic->vga_info.resolution_height = 0;
        fpga_statistic->vga_info.frame_rate = 0;

        fpga_statistic->event_bits |= FPGA_EVENT_VGA_HOTPLUG;
    }

    return 0;
}

//处理sdi热拔插中断
static int sdi_hotplug_process(void)
{
    int ret = 0;
    unsigned char val;

    ret = fpga_read_byte(FPGA_VIDEO_IN_DETECT_REG, &val);
    if (ret != 0) {
        fpga_log("read reg[0x%x] failed \n", FPGA_VIDEO_IN_DETECT_REG);
        return -1;
    }

    fpga_statistic->sdi_info.in = val & FPGA_SDI_IN_BIT ? 1 : 0;
    if (fpga_statistic->sdi_info.in) {
        fpga_statistic->sdi_info.resolution_width = 0;
        fpga_statistic->sdi_info.resolution_height = 0;
        fpga_statistic->sdi_info.frame_rate = 0;

        fpga_read_byte(FPGA_SDI_WIDTH_H_REG, &val);
        fpga_statistic->sdi_info.resolution_width |= (val << 8);
        fpga_read_byte(FPGA_SDI_WIDTH_L_REG, &val);
        fpga_statistic->sdi_info.resolution_width |= val;

        fpga_read_byte(FPGA_SDI_HEIGHT_H_REG, &val);
        fpga_statistic->sdi_info.resolution_height |= (val << 8);
        fpga_read_byte(FPGA_SDI_HEIGHT_L_REG, &val);
        fpga_statistic->sdi_info.resolution_height |= val;

        fpga_read_byte(FPGA_SDI_FRAME_RATE_REG, &val);
        fpga_statistic->sdi_info.frame_rate = val;

        fpga_statistic->event_bits |= FPGA_EVENT_SDI_HOTPLUG;
    } else {
        fpga_statistic->sdi_info.resolution_width = 0;
        fpga_statistic->sdi_info.resolution_height = 0;
        fpga_statistic->sdi_info.frame_rate = 0;

        fpga_statistic->event_bits |= FPGA_EVENT_SDI_HOTPLUG;
    }

    return 0;
}

//处理POE相关中断
static int poe_process(void)
{
    fpga_log("poe not support, to be continue... \n");

    return 0;
}

static void fpga_interrupt_tasklet_func(struct work_struct* work)
{
    int ret;
    int i = 0, j = 0;
    unsigned char val;
    int vga_hotplug = 0, sdi_hotplug = 0;           //标记是否有拔插事件发生
    unsigned char interrupt_status;                 //暂存中断状态寄存器
    unsigned char vga_in_fliter[FPGA_FLITER_MAGIC]; //vga in拔插消抖
    unsigned char sdi_in_fliter[FPGA_FLITER_MAGIC]; //sdi in拔插消抖

    ret = fpga_read_byte(FPGA_INTER_STATUS_REG, &interrupt_status);
    if (ret != 0) {
        fpga_log("read interrupt status reg failed \n");
        return;
    }

    fpga_dbg("interrupt_status: 0x%x \n", interrupt_status);

    //如果有拔插事件, 那么进行消抖处理
    if ((interrupt_status & FPGA_INTRT_VGA_HOTPLUG_BIT) || (interrupt_status & FPGA_INTER_SDI_HOTPLUG_BIT)) {
        while (i < 100) {
            fpga_read_byte(FPGA_VIDEO_IN_DETECT_REG, &val);

            if (interrupt_status & FPGA_INTRT_VGA_HOTPLUG_BIT) {
                vga_in_fliter[i % FPGA_FLITER_MAGIC] = val & FPGA_VGA_IN_BIT ? 1 : 0;
            }
            if (interrupt_status & FPGA_INTER_SDI_HOTPLUG_BIT) {
                sdi_in_fliter[i % FPGA_FLITER_MAGIC] = val & FPGA_SDI_IN_BIT ? 1 : 0;
            }

            i += 1;
            if (i >= FPGA_FLITER_MAGIC) {
                for (j = 0; j < FPGA_FLITER_MAGIC - 1; j++) {
                    if (interrupt_status & FPGA_INTRT_VGA_HOTPLUG_BIT) {
                        if (vga_in_fliter[j] != vga_in_fliter[j + 1]) {
                            usleep_range(30000, 31000);     //隔30ms读一次状态
                            continue;                       //连接状态不稳定, 等待稳定
                        }
                    }

                    if (interrupt_status & FPGA_INTER_SDI_HOTPLUG_BIT) {
                        if (sdi_in_fliter[j] != sdi_in_fliter[j + 1]) {
                            usleep_range(30000, 31000);     //隔30ms读一次状态
                            continue;                       //连接状态不稳定, 等待连接稳定
                        }
                    }
                }
            } else {
                usleep_range(30000, 31000);     //隔30ms读一次状态
                continue;                       //连接状态不稳定, 等待连接稳定
            }

            //连接状态稳定后, 出路拔插状态
            if (interrupt_status & FPGA_INTRT_VGA_HOTPLUG_BIT) {
                vga_hotplug = 1;
            }

            if (interrupt_status & FPGA_INTER_SDI_HOTPLUG_BIT) {
                sdi_hotplug = 1;
            }

            break;
        }
    }

    if (interrupt_status & FPGA_INTER_SDI_C_ABNOR_BIT) {
        sdi_c_abnormal_process();
    }

    if (interrupt_status & FPGA_INTER_SDI_Y_ABNOR_BIT) {
        sdi_y_abnormal_process();
    }

    if (interrupt_status & FPGA_INTER_POE) {
        poe_process();
    }

    //等待FPGA统计帧率稳定
    if (vga_hotplug || sdi_hotplug) {
        usleep_range(1000000, 1000000);
    }

    //重新读取视频信息
    if (vga_hotplug) {
        fpga_log("detect vga hotplug \n");
        ret = vga_hotplug_process();
        if (ret != 0) {
            fpga_log("process vga hotplug interrupt failed \n");
        }
    }

    if (sdi_hotplug) {
        fpga_log("detect sdi hotplug \n");
        ret = sdi_hotplug_process();
        if (ret != 0) {
            fpga_log("process sdi hotplug interrupt failed \n");
        }
    }

    if (fpga_statistic->event_bits != 0) {
        post_nl_msg(FPGA_NET_LINK_MSG, STAR_NET_DRIVER_MSG_GRP);    //向应用上报消息
    }

    fpga_statistic->interr_cnt += 1;

    //清除中断
    fpga_write_byte(FPGA_INTER_CLEAR_REG, 0x00);
}

irqreturn_t fpga_interrupt_handler(int irq, void* dev_id)
{
    //由于读取fpga状态寄存器操作包含锁, 所以放到下半部操作
    schedule_work(&fpga_interrupt_tasklet);
    return IRQ_HANDLED;
}

static int fpga_monitor_probe(struct platform_device* pdev)
{
    int ret;
    int i2c_bus;            //fpga所接的i2c总线
    int i2c_addr;           //fpga的设备地址
    int spi_bus;            //fpga使用的spi flash所接的spi总线
    int spi_csn;            //fpga使用的spi flash所接的片选脚
    int spi_clk;            //fpga使用spi fpash所接的spi总线的时钟频率
    int retry_times = 300;  //总共等待3秒
    struct spi_master* spi_master;  //fpga所使用的spi flash所使用的spi master

    //解析设备树, 并创建fpga monitor用于监控fpga的状态
    if (pdev->dev.of_node == NULL) {
        fpga_log("can not find fpga_monitor device node, please check your device tree \n");
        return -1;
    }

    fpga_monitor = (fpga_monitor_t*)kzalloc(sizeof(fpga_monitor_t), GFP_KERNEL);
    if (fpga_monitor == NULL) {
        fpga_log("alloc memory for fpga_monitor failed \n");
        return -1;
    }

    fpga_statistic = (fpga_statistic_t*)kzalloc(sizeof(fpga_statistic_t), GFP_KERNEL);
    if (fpga_statistic == NULL) {
        fpga_log("alloc memory for fpga_statistic failed \n");
        goto err1;
    }

    //解析设备树
    ret = of_property_read_u32(pdev->dev.of_node, "intr_pin", &fpga_monitor->intr_pin);
    if (ret != 0) {
        fpga_log("get attribute(intr_pin) failed, please check your device tree \n");
        goto err2;
    }

    ret = of_property_read_u32(pdev->dev.of_node, "start_up_pin", &fpga_monitor->start_up_pin);
    if (ret != 0) {
        fpga_log("get attribute(start_up_pin) failed, please check your device tree \n");
        goto err2;
    }

    ret = of_property_read_u32(pdev->dev.of_node, "flash_ntf_pin", &fpga_monitor->flash_ntf_pin);
    if (ret != 0) {
        fpga_log("get attribute(flash_ntf_pin) failed, please check your device tree \n");
        goto err2;
    }

    ret = of_property_read_u32_index(pdev->dev.of_node, "i2c_info", 0, &i2c_bus);
    if (ret != 0) {
        fpga_log("get number of i2c bus failed, please check your device tree \n");
        goto err2;
    }

    ret = of_property_read_u32_index(pdev->dev.of_node, "i2c_info", 1, &i2c_addr);
    if (ret != 0) {
        fpga_log("get i2c device address of fpga failed, please check your device tree \n");
        goto err2;
    }

    ret = of_property_read_u32_index(pdev->dev.of_node, "spi_info", 0, &spi_bus);
    if (ret != 0) {
        fpga_log("get number of spi bus failed, please check your device tree \n");
        goto err2;
    }
    fpga_log("spi_bus:%d \n", spi_bus);

    ret = of_property_read_u32_index(pdev->dev.of_node, "spi_info", 1, &spi_csn);
    if (ret != 0) {
        fpga_log("get spi csn of flash failed, please check your device tree \n");
        goto err2;
    }
    fpga_log("spi_csn:%d \n", spi_csn);

    ret = of_property_read_u32_index(pdev->dev.of_node, "spi_info", 2, &spi_clk);
    if (ret != 0) {
        fpga_log("get spi clock frequence of flash failed, please check your device tree \n");
        goto err2;
    }
    fpga_log("spi_clk:%d \n", spi_clk);

    //申请相关资源
    gpio_request(fpga_monitor->start_up_pin, NULL);
    gpio_request(fpga_monitor->intr_pin, NULL);
    gpio_request(fpga_monitor->flash_ntf_pin, NULL);
    gpio_direction_input(fpga_monitor->flash_ntf_pin);

    fpga_monitor->adapter = i2c_get_adapter(i2c_bus);
    if (fpga_monitor->adapter == NULL) {
        fpga_log("get i2c adapter for fpga failed \n");
        goto err3;
    }

    strncpy(i2c_info.type, "fpga_i2c", I2C_NAME_SIZE - 1);
    i2c_info.addr = i2c_addr >> 1;

    fpga_monitor->client = i2c_new_device(fpga_monitor->adapter, &i2c_info);
    if (fpga_monitor->client == NULL) {
        fpga_log("create i2c device for fpga failed \n");
        goto err4;
    }

#if 0
    //创建spi资源
    spi_master = spi_busnum_to_master(spi_bus);
    if (spi_master == NULL) {
        fpga_log("get spi master of spi flash failed \n");
        goto err5;
    }

    //创建依附于spi_bus所对应的spi master的spi device
    fpga_monitor->spi_dev = spi_alloc_device(spi_master);
    if (fpga_monitor->spi_dev == NULL) {
        fpga_log("create spi device for spi flash failed \n");
        goto err5;
    }

    //初始化spi flash所使用的片选信号
    fpga_monitor->spi_dev->chip_select = spi_csn;
    fpga_monitor->spi_dev->max_speed_hz = spi_clk;

    //将spi device注册到master上
    ret = spi_add_device(fpga_monitor->spi_dev);
    if (ret != 0) {
        fpga_log("register spi device to spi master fail with %d \n", ret);
        goto err6;
    }
#endif

    spi_register_driver(&spi_flash_driver);

    //等待FPGA启动完成
    gpio_direction_input(fpga_monitor->start_up_pin);
    while (retry_times) {
        retry_times -= 1;
        ret = gpio_get_value(fpga_monitor->start_up_pin);

        if (ret) {
            fpga_log("fpga start up successful \n");
            break;
        } else if(ret == 0 && retry_times == 0) {
            fpga_log("fpga start up failed \n");
            break;
        }

        udelay(10000);
    }

    //初始化FPGA统计信息
    ret = fpga_statistic_init();
    if (ret != 0) {
        fpga_log("do fpga statistic init failed \n");
    }

    //注册中断
    INIT_WORK(&fpga_interrupt_tasklet, fpga_interrupt_tasklet_func);
    gpio_direction_input(fpga_monitor->intr_pin);   //配置中断引脚为输入
    fpga_monitor->irq = gpio_to_irq(fpga_monitor->intr_pin);
    ret = request_irq(fpga_monitor->irq, fpga_interrupt_handler, IRQF_SHARED | IRQF_TRIGGER_RISING, "fpga_intr", fpga_monitor);

    //创建/proc/fpga_monitor文件
    proc_create("fpga_monitor", S_IRWXU | S_IRWXG | S_IRWXO, NULL, &fpga_fops);

    return 0;

//err6:
//    kfree(fpga_monitor->spi_dev);
//err5:
    i2c_unregister_device(fpga_monitor->client);
err4:
    i2c_put_adapter(fpga_monitor->adapter);
err3:
    gpio_free(fpga_monitor->flash_ntf_pin);
    gpio_free(fpga_monitor->intr_pin);
    gpio_free(fpga_monitor->start_up_pin);
err2:
    kfree(fpga_statistic);
err1:
    kfree(fpga_monitor);
    return -1;
}

static int fpga_monitor_remove(struct platform_device* pdev)
{
    if (fpga_monitor == NULL)
        return 0;

    remove_proc_entry("fpga_monitor", NULL);
    spi_unregister_driver(&spi_flash_driver);
    free_irq(fpga_monitor->irq, fpga_monitor);
    i2c_unregister_device(fpga_monitor->client);
    i2c_put_adapter(fpga_monitor->adapter);
    gpio_free(fpga_monitor->flash_ntf_pin);
    gpio_free(fpga_monitor->intr_pin);
    gpio_free(fpga_monitor->start_up_pin);
    kfree(fpga_statistic);
    kfree(fpga_monitor);

    return 0;
}

static struct of_device_id fpga_monitor_ids[] = {
    {.compatible = "fpga_monitor"},
    {}
};

static struct platform_driver fpga_monitor_drv = {
    .driver = {
        .name = "fpga_monitor",
        .of_match_table = of_match_ptr(fpga_monitor_ids)
    },
    .probe = fpga_monitor_probe,
    .remove = fpga_monitor_remove
};

static int fpga_monitor_init(void)
{
    return platform_driver_register(&fpga_monitor_drv);
}

static void fpga_monitor_exit(void)
{
    platform_driver_unregister(&fpga_monitor_drv);
}

module_init(fpga_monitor_init);
module_exit(fpga_monitor_exit);

MODULE_AUTHOR("WUJING");
MODULE_DESCRIPTION("FPGA MONITOR DRIVER");
MODULE_LICENSE("GPL");
