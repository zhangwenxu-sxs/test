/**
 * 说明：该驱动用于实现oled的初始化, 并提供接口供应用
 * 程序操作oled(如显示指定字符、关闭oled、设置切换效
 * 果等操作)
 *
 * 注：oled 连接SPI-1, 以CSN0作为片选引脚
 *    oled 复位引脚 GPIO3_6
 *    oled 数据/命令 控制引脚GPIO17_5  1：发数据    0：发命令
 *    oled 电源控制引脚 GPIO5_5
 * */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/netlink.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/switch.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <asm/io.h>

#include "sn_netlink.h"
#include "oled_ssd1322.h"

static int oled_probe(struct spi_device *spi);
static int oled_remove(struct spi_device *spi);

static int oled_reset(void);
static int oled_power_on(void);
static int oled_power_down(void);
static int oled_do_init(void);
static int oled_set_disp_mode(oled_disp_mode_e mode);

static int oled_spi_write_bytes(unsigned char* buffer, unsigned int length, oled_tx_type_e type);

static int oled_open(struct inode* inode, struct file* file);
static int oled_release(struct inode* inode, struct file* file);
static ssize_t oled_read(struct file* file, char __user * buffer, size_t len, loff_t* offset);
static ssize_t oled_write(struct file* file, const char __user * buffer, size_t len, loff_t* offset);
static long oled_ioctl(struct file* file, unsigned int cmd, unsigned long args);

//这个函数将两阶的点阵字库转成16阶的点阵字库使用的字库
static void oled_convert_dot_matrix(unsigned char* in, volatile unsigned char* out);
//将一个unicode字符的点阵数据写到framebuffer中
static void oled_render_char(render_char_t* ch);
//将framebuffer中的数据更新到oled的GDDRAM中显示
static void oled_flush_fb(void);

static struct cdev oled_chrdev;     //给用户空间提供的字符设备
static struct class* oled_class;    //
static oled_ssd1322_t* oled = NULL; //
static unsigned char ch_lib[128];   //存储某个字符的点阵字库

static struct of_device_id oled_id = {
    .compatible = "oled-ssd1322"
};

static struct file_operations oled_fops = {
    .open = oled_open,
    .release = oled_release,
    .read = oled_read,
    .write = oled_write,
    .unlocked_ioctl = oled_ioctl
};

static struct spi_driver oled_driver = {
    .driver = {
        .name = "spi-oled",
        .of_match_table = &oled_id
    },
    .probe = oled_probe,
    .remove = oled_remove,
};

static int oled_probe(struct spi_device *spi)
{
    int ret;

    oled_dbg("do oled probe... \n");

    if (oled != NULL) {
        oled_log("oled already probed \n");
        goto err1;
    }

    oled = kzalloc(sizeof(oled_ssd1322_t), GFP_KERNEL);
    if (oled == NULL) {
        oled_log("alloc memory for oled failed \n");
        goto err1;
    }

    oled->spi_dev = spi;                //初始化spi设备
    oled->power_on = 0;                 //标志当前oled还未完成上电时序
    oled->speed_hz = spi->max_speed_hz; //获取SPI总线的速度
    mutex_init(&oled->tx_mutex);        //初始化锁
    mutex_init(&oled->fb_mutex);        //初始化锁

    ret = of_property_read_u32(spi->dev.of_node, "reset-pin", &oled->reset_pin);
    if (ret != 0) {
        oled_log("get property[reset-pin] failed, please check your device tree \n");
        goto err2;
    }

    ret = of_property_read_u32(spi->dev.of_node, "power-pin", &oled->power_pin);
    if (ret != 0) {
        oled_log("get property[power-pin] failed, please check your device tree \n");
        goto err2;
    }

    ret = of_property_read_u32(spi->dev.of_node, "command-data-pin", &oled->cmd_data_pin);
    if (ret != 0) {
        oled_log("get property[command-data-pin] failed, please check your device tree \n");
        goto err2;
    }

    oled_dbg("oled:spi%d-%d \n", spi->master->bus_num, spi->chip_select);
    oled_dbg("oled->speed_hz:%d \n", oled->speed_hz);
    oled_dbg("oled->reset_pin:GPIO%d_%d \n", oled->reset_pin / 8, oled->reset_pin % 8);
    oled_dbg("oled->power_pin:GPIO%d_%d \n", oled->power_pin / 8, oled->power_pin % 8);
    oled_dbg("oled->cmd_data_pin:GPIO%d_%d \n", oled->cmd_data_pin / 8, oled->cmd_data_pin % 8);

    //申请GPIO资源
    gpio_request(oled->reset_pin, NULL);
    gpio_request(oled->power_pin, NULL);
    gpio_request(oled->cmd_data_pin, NULL);
    //初始化GPIO默认输出电平
    gpio_direction_output(oled->reset_pin, 1);
    gpio_direction_output(oled->power_pin, 0);
    gpio_direction_output(oled->cmd_data_pin, 0);

    //字符设备相关初始化
    ret = alloc_chrdev_region(&oled->cdev, 0, 1, "oled-region");    //分配字符设备号
    if (ret != 0) {
        oled_log("alloc char device number for oled failed \n");
        goto err3;
    }

    cdev_init(&oled_chrdev, &oled_fops);            //初始化字符设备的文件操作函数
    ret = cdev_add(&oled_chrdev, oled->cdev, 1);    //将字符设备添加如相应的内核数据结构
    if (ret != 0) {
        oled_log("register char device to kernel for oled failed \n");
        goto err4;
    }

    oled_class = class_create(THIS_MODULE, "oled_class");
    if (IS_ERR(oled_class)) {
        oled_log("crate class for oled failed \n");
        goto err5;
    }

    if (IS_ERR(device_create(oled_class, NULL, oled->cdev, NULL, "oled-ssd1322"))) {
        oled_log("create user space device node failed for oled failed \n");
        goto err6;
    }

    oled_power_on();
    oled_do_init();
    oled_flush_fb();

    return 0;

err6:
    class_destroy(oled_class);
err5:
    cdev_del(&oled_chrdev);
err4:
    unregister_chrdev_region(oled->cdev, 1);
err3:
    gpio_free(oled->cmd_data_pin);
    gpio_free(oled->power_pin);
    gpio_free(oled->reset_pin);
err2:
    kfree(oled);
    oled = NULL;
err1:
    return -1;
}

static int oled_remove(struct spi_device *spi)
{
    oled_dbg("do oled remove... \n");

    if (oled == NULL) {
        return 0;
    }

    device_destroy(oled_class, oled->cdev);
    class_destroy(oled_class);
    cdev_del(&oled_chrdev);
    unregister_chrdev_region(oled->cdev, 1);
    gpio_free(oled->cmd_data_pin);
    gpio_free(oled->power_pin);
    gpio_free(oled->reset_pin);
    if (oled->font_lib != NULL) {
        kfree(oled->font_lib);
    }
    kfree(oled);
    oled = NULL;

    return 0;
}

static int oled_reset()
{
    if (oled == NULL) {
        oled_log("oled not probed, please checkout your device tree \n");
        return -1;
    }

    gpio_direction_output(oled->reset_pin, 0);
    usleep_range(10000, 11000);
    gpio_direction_output(oled->reset_pin, 1);

    return 0;
}

static int oled_power_on()
{
    if (oled == NULL) {
        oled_log("oled not probed, please checkout your device tree \n");
        return -1;
    }

    if (oled->power_on) {
        oled_log("oled already power on \n");
        return 0;
    }

    gpio_direction_output(oled->power_pin, 0);
    usleep_range(10000, 11000);

    oled_reset();
    usleep_range(10000, 11000);

    gpio_direction_output(oled->power_pin, 1);

    oled->power_on = 1;

    return 0;
}

static int oled_power_down()
{
    if (oled == NULL) {
        oled_log("oled not probed, please checkout your device tree \n");
        return -1;
    }

    if (!oled->power_on) {
        oled_log("oled already power down \n");
        return 0;
    }

    gpio_direction_output(oled->power_pin, 0);
    oled->power_on = 0;

    return 0;
}

static int oled_spi_write_bytes(unsigned char* buffer, unsigned int length, oled_tx_type_e type)
{
    int ret;
    struct spi_message msg;
    struct spi_transfer tx = {
        .tx_buf = buffer,
        .len = length,
        .speed_hz = oled->speed_hz
    };

    if (oled == NULL) {
        oled_log("oled not probed, please checkout your device tree \n");
        return -1;
    }

    gpio_direction_output(oled->cmd_data_pin, type == OLED_TX_DATA ? 1 : 0);

    spi_message_init(&msg);
    spi_message_add_tail(&tx, &msg);

    ret = spi_sync(oled->spi_dev, &msg);

    if (ret == 0)
        return msg.actual_length;

    oled_log("oled write spi failed \n");
    return -1;
}

static int oled_do_init()
{
    int i;

    oled_transfer_t init_cmds[] = {
        {0xfd, OLED_TX_COMMAND},
        {0x12, OLED_TX_DATA},
        {0xae, OLED_TX_COMMAND},
        {0x15, OLED_TX_COMMAND},
        {0x1c, OLED_TX_DATA},
        {0x5b, OLED_TX_DATA},
        {0x75, OLED_TX_COMMAND},
        {0x00, OLED_TX_DATA},
        {0x3f, OLED_TX_DATA},
        {0xa0, OLED_TX_COMMAND},
        {0x14, OLED_TX_DATA},
        {0x11, OLED_TX_DATA},
        {0xa1, OLED_TX_COMMAND},
        {0x00, OLED_TX_DATA},
        {0xa2, OLED_TX_COMMAND},
        {0x00, OLED_TX_DATA},
        {0xab, OLED_TX_COMMAND},
        {0x01, OLED_TX_DATA},
        {0Xb1, OLED_TX_COMMAND},
        {0xe2, OLED_TX_DATA},
        {0xb3, OLED_TX_COMMAND},
        {0x91, OLED_TX_DATA},
        {0xb4, OLED_TX_COMMAND},
        {0xa0, OLED_TX_DATA},
        {0xfd, OLED_TX_DATA},
        {0xb5, OLED_TX_COMMAND},
        {0X00, OLED_TX_DATA},
        {0xb6, OLED_TX_COMMAND},
        {0x08, OLED_TX_DATA},
        {0xb9, OLED_TX_COMMAND},
        {0xbb, OLED_TX_COMMAND},
        {0x1f, OLED_TX_DATA},
        {0xbe, OLED_TX_COMMAND},
        {0x07, OLED_TX_DATA},
        {0xc1, OLED_TX_COMMAND},
        {0x9f, OLED_TX_DATA},
        {0xc7, OLED_TX_COMMAND},
        {0x0f, OLED_TX_DATA},
        {0xca, OLED_TX_COMMAND},
        {0x3f, OLED_TX_DATA},
        {0xd1, OLED_TX_COMMAND},
        {0x82, OLED_TX_DATA},
        {0x20, OLED_TX_DATA},
        {0xa6, OLED_TX_COMMAND},
        {0xaf, OLED_TX_COMMAND}
    };

    if (oled == NULL) {
        oled_log("oled not probed, please checkout your device tree \n");
        return -1;
    }

    mutex_lock(&oled->tx_mutex);
    for (i = 0; i < sizeof(init_cmds) / sizeof(oled_transfer_t); i++) {
        oled_spi_write_bytes(&init_cmds[i].data, 1, init_cmds[i].type);
    }
    mutex_unlock(&oled->tx_mutex);

    usleep_range(20000, 21000);

    return 0;
}

static int oled_set_disp_mode(oled_disp_mode_e mode)
{
    oled_transfer_t cmd = {
        .type = OLED_TX_COMMAND
    };

    switch (mode) {
    case OLED_DISP_NORMAL:
        cmd.data = 0xa6;
        mutex_lock(&oled->tx_mutex);
        oled_spi_write_bytes(&cmd.data, 1, cmd.type);
        mutex_unlock(&oled->tx_mutex);
        break;
    case OLED_DISP_ON:
        cmd.data = 0xa5;
        mutex_lock(&oled->tx_mutex);
        oled_spi_write_bytes(&cmd.data, 1, cmd.type);
        mutex_unlock(&oled->tx_mutex);
        break;
    case OLED_DISP_OFF:
        cmd.data = 0xa4;
        mutex_lock(&oled->tx_mutex);
        oled_spi_write_bytes(&cmd.data, 1, cmd.type);
        mutex_unlock(&oled->tx_mutex);
        break;
    case OLED_DISP_INVERSE:
        cmd.data = 0xa7;
        mutex_lock(&oled->tx_mutex);
        oled_spi_write_bytes(&cmd.data, 1, cmd.type);
        mutex_unlock(&oled->tx_mutex);
        break;
    default:
        oled_log("invalid oled display mode \n");
        return -1;
    }

    return 0;
}

static int oled_open(struct inode* inode, struct file* file)
{
    return 0;
}

static int oled_release(struct inode* inode, struct file* file)
{
    return 0;
}

static ssize_t oled_read(struct file* file, char __user * buffer, size_t len, loff_t* offset)
{
    int i, j;
    size_t ret;

    if (len != sizeof(oled->framebuffer)) {
        oled_log("user must read one framebuffer size data to oled framebuffer one time \n");
        return -1;
    }

    mutex_lock(&oled->fb_mutex);
    ret = copy_to_user(buffer, oled->framebuffer, len);
    mutex_unlock(&oled->fb_mutex);

    return len - ret;
}

static ssize_t oled_write(struct file* file, const char __user * buffer, size_t len, loff_t* offset)
{
    int i, j;
    size_t ret;

    if (len != sizeof(oled->framebuffer)) {
        oled_log("user must write one framebuffer size data to oled framebuffer one time \n");
        return -1;
    }

    mutex_lock(&oled->fb_mutex);
    ret = copy_from_user(oled->framebuffer, buffer, len);
    mutex_unlock(&oled->fb_mutex);

    return len - ret;
}

static long oled_ioctl(struct file* file, unsigned int cmd, unsigned long args)
{
    int ret;

    switch (cmd) {
    case CMD_OLED_POWER_ON:
        oled_dbg("do CMD_OLED_POWER_ON command \n");
        oled_power_on();
        oled_do_init();
        break;
    case CMD_OLED_POWER_OFF:
        oled_dbg("do CMD_OLED_POWER_OFF command \n");
        oled_power_down();
        break;
    case CMD_OLED_SET_DISP_MODE:
        oled_dbg("do CMD_OLED_SET_DISP_MODE command \n");
        oled_set_disp_mode((oled_disp_mode_e)args);
        break;
    case CMD_OLED_RENDER_CHAR:
        oled_dbg("do CMD_OLED_RENDER_CHAR command \n");
        render_char_t* ch = kzalloc(sizeof(render_char_t), GFP_KERNEL);
        ret = copy_from_user(ch, (void*)args, sizeof(render_char_t));
        if (ret == 0)
            oled_render_char(ch);
        kfree(ch);
        break;
    case CMD_OLED_CLEAR_FB:
        oled_dbg("do CMD_OLED_CLEAR_FB command \n");
        mutex_lock(&oled->fb_mutex);
        memset(oled->framebuffer, 0x00, sizeof(oled->framebuffer));
        mutex_unlock(&oled->fb_mutex);
        break;
    case CMD_OLED_FLUSH_FB:
        oled_dbg("do CMD_OLED_FLUSH_FB command \n");
        oled_flush_fb();
        break;
    case CMD_OLED_SET_FONT_LIB:
        oled_dbg("do CMD_OLED_SET_FONT_LIB command \n");

        if (oled->font_lib != NULL) {
            oled_log("already set font library \n");
            return 0;
        }

        font_lib_config_t* font_lib = (font_lib_config_t*)args;
        oled->font_lib = kzalloc(font_lib->length, GFP_KERNEL);
        if (oled->font_lib == NULL) {
            oled_log("alloc memory for font lib failed \n");
            return -1;
        }

        ret = copy_from_user(oled->font_lib, font_lib->font_lib, font_lib->length);
        if (ret != 0) {
            oled_log("copy unicode font library from user space failed \n");
            kfree(oled->font_lib);
            oled->font_lib = NULL;
            return -1;
        }

        break;
    case CMD_OLED_GET_OLED_INFO:
        oled_dbg("do CMD_OLED_GET_RESOLUTION command \n");
        oled_info_t* info = (oled_info_t*)args;
        info->x_res = OLED_COL_NUM;     //x轴像素数
        info->y_res = OLED_ROW_NUM;     //y轴像素数
        info->depth = OLED_COLOR_DEPTH; //一个像素由4位数据表示，即每个像素能显示16种不同的亮度
        info->fb_size = sizeof(oled->framebuffer);  //oled的framebuffer大小
        break;
    case CMD_OLED_SET_DISP_ANIMATION:
        oled_dbg("do CMD_OLED_SET_DISP_ANIMATION command \n");
        oled->disp_animation = args > OLED_ANIMATION_ROLL ? OLED_ANIMATION_ROLL : args;
        break;
    case CMD_OLED_SET_SWITCH_ANIMATION:
        oled_dbg("do CMD_OLED_SET_SWITCH_ANIMATION command \n");
        oled->switch_animation = args > OLED_ANIMATION_BOTTM_IN ? OLED_ANIMATION_BOTTM_IN : args;
        break;
    }

    return 0;
}

/**
 * 说明：
 *     1、我们使用的点阵字库是1bit代表一个像素的亮灭，使用的oled是用4bit控制一个像素的亮灭
 *     2、我们使用的字库是16*16的点阵字库，所以一个字符所占的字节数是16*16pixel / 8 = 32bytes
 *     3、对于16*16的字符，我们使用的oled需要16*16/2 = 128bytes, 所以需要转换
 * */
static void oled_convert_dot_matrix(unsigned char* in, volatile unsigned char* out)
{
    int i, j, k;
    unsigned char tmp;

    /**
     * 这部分转换需要参考ssd1322液晶控制芯片手册的10.1.6章节
     * */
    for (i = 0; i < 16; i++) {
        for (k = 0; k < 2; k++) {
            for (j = 0; j < 8; j++) {
                if ((*in) & 1 << (7 - j)) {
                    (*out) |= ((j % 2) == 0 ? 0xf0 : 0x0f);
                } else {
                    (*out) &= ((j % 2) == 0 ? 0x0f : 0xf0);
                }
                if (j % 2 == 1) {
                    out += 1;
                }
            }

            in += 1;
        }
    }
}

//将一个unicode字符的点阵数据写到framebuffer中
static void oled_render_char(render_char_t* ch)
{
    int i, j, k, z;                     //循环控制变量
    int start_x = (ch->x_pos / 2) * 2;  //字符串显示的起始x坐标, 两像素对齐
    int start_y = ch->y_pos;            //字符串显示的起始y坐标

    if (oled->font_lib == NULL) {
        oled_log("oled driver haven't set font library, please use ioctl(CMD_OLED_SET_FONT_LIB) to set it first \n");
        return;
    }

    for (z = 0; z < ch->cnt; z++) {
        //将一像素1bit的字库数据转成一像素4bits的字库数据
        memset(ch_lib, 0, sizeof(ch_lib));
        oled_convert_dot_matrix(oled->font_lib + ch->ch[z] * 32, ch_lib);

        //剩下的像素数显示不完一个字符则换行显示
        if (start_x + 16 > OLED_COL_NUM) {
            start_x = 0;
            start_y += 16;
        }

        mutex_lock(&oled->fb_mutex);
        for (i = 0; i < 16; i++) {
            k = start_y + i;
            if (k >= OLED_ROW_NUM) {
                break;
            }

            for(j = 0; j < 8; j++) {
                if (start_x / 2 + j < OLED_COL_NUM / 2) {
                    oled->framebuffer[k][start_x / 2 + j] = ch_lib[i * 8 + j];
                } else {
                    break;
                }
            }
        }
        mutex_unlock(&oled->fb_mutex);
        start_x += 16;  //我们使用了16*16的点阵字库, 显示完x偏移16像素后显示下一个字符
    }
}

//将framebuffer中的数据更新到oled的GDDRAM中显示
static void oled_flush_fb(void)
{
    oled_transfer_t transfer;

    mutex_lock(&oled->tx_mutex);

    //配置列起始地址和结束地址
    transfer.data = 0x15;
    transfer.type = OLED_TX_COMMAND;
    oled_spi_write_bytes(&transfer.data, 1, transfer.type);
    transfer.data = 0x1c;
    transfer.type = OLED_TX_DATA;
    oled_spi_write_bytes(&transfer.data, 1, transfer.type);
    transfer.data = 0x5b;
    transfer.type = OLED_TX_DATA;
    oled_spi_write_bytes(&transfer.data, 1, transfer.type);
    //配置行起始地址和结束地址
    transfer.data = 0x75;
    transfer.type = OLED_TX_COMMAND;
    oled_spi_write_bytes(&transfer.data, 1, transfer.type);
    transfer.data = 0x00;
    transfer.type = OLED_TX_DATA;
    oled_spi_write_bytes(&transfer.data, 1, transfer.type);
    transfer.data = 0x3f;
    transfer.type = OLED_TX_DATA;
    oled_spi_write_bytes(&transfer.data, 1, transfer.type);

    //写入写RAM命令
    transfer.data = 0x5c;
    transfer.type = OLED_TX_COMMAND;
    oled_spi_write_bytes(&transfer.data, 1, transfer.type);
    //将framebuffer中的数据写入oled
    mutex_lock(&oled->fb_mutex);
    oled_spi_write_bytes((unsigned char*)oled->framebuffer, sizeof(oled->framebuffer), OLED_TX_DATA);
    oled_dbg("sizeof(oled->framebuffer):%d  %d \n", sizeof(oled->framebuffer), oled->framebuffer[1] - oled->framebuffer[0]);
    mutex_unlock(&oled->fb_mutex);

    mutex_unlock(&oled->tx_mutex);
}

static int oled_ssd1322_init(void)
{
    oled_dbg("do oled init... \n");
    return spi_register_driver(&oled_driver);
}

static void oled_ssd1322_exit(void)
{
    if (oled == NULL) {
        return;
    }

    oled_power_down();
    device_destroy(oled_class, oled->cdev);
    class_destroy(oled_class);
    cdev_del(&oled_chrdev);
    unregister_chrdev_region(oled->cdev, 1);
    gpio_free(oled->cmd_data_pin);
    gpio_free(oled->power_pin);
    gpio_free(oled->reset_pin);
    if (oled->font_lib != NULL) {
        kfree(oled->font_lib);
    }
    kfree(oled);
    oled = NULL;

    spi_unregister_driver(&oled_driver);
}

module_init(oled_ssd1322_init);
module_exit(oled_ssd1322_exit);

MODULE_AUTHOR("WUJING");
MODULE_DESCRIPTION("OLED SSD1322 DRIVER");
MODULE_LICENSE("GPL");
