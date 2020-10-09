#ifndef __FPGA_MONITOR_H
#define __FPGA_MONITOR_H

#include <linux/nls.h>
#include <linux/mutex.h>

#define FPGA_MONITOR_DBG 1

#define fpga_log(fmt, args...) printk("[FPGA_MONITOR][FUNC:%s][LINE:%d]"fmt, __FUNCTION__, __LINE__, ##args)

#if FPGA_MONITOR_DBG
#define fpga_dbg(fmt, args...) printk("[FPGA_MONITOR][FUNC:%s][LINE:%d]"fmt, __FUNCTION__, __LINE__, ##args)
#else
#define fpga_dbg(fmt, args...)
#endif

/**
 * 寄存器地址定义
 * */
#define FPGA_VERSION_REG            0x01    //FPGA版本寄存器
#define FPGA_SDI_FRAME_RATE_REG     0x02    //FPGA SDI输入帧率寄存器
#define FPGA_VGA_FRAME_RATE_REG     0x03    //FPGA VGA输入帧率寄存器
#define FPGA_VIDEO_IN_DETECT_REG    0x07    //FPGA 视频输入状态检测寄存器
#define FPGA_VIDEO_IN_SWITCH_REG    0x08    //FPGA 视频输入源选择寄存器
#define FPGA_INTER_STATUS_REG       0x06    //FPGA 中断状态寄存器
#define FPGA_SDI_WIDTH_H_REG        0x09    //FPGA SDI输入行像素高八位
#define FPGA_SDI_WIDTH_L_REG        0x0a    //FPGA SDI输入行像素低八位
#define FPGA_SDI_HEIGHT_H_REG       0x0b    //FPGA SDI输入列像素高八位
#define FPGA_SDI_HEIGHT_L_REG       0x0c    //FPGA SDI输入列像素低八位
#define FPGA_VGA_WIDTH_H_REG        0x0d    //FPGA VGA输入行像素高八位
#define FPGA_VGA_WIDTH_L_REG        0x0e    //FPGA VGA输入行像素低八位
#define FPGA_VGA_HEIGHT_H_REG       0x0f    //FPGA VGA输入列像素高八位
#define FPGA_VGA_HEIGHT_L_REG       0x10    //FPGA VGA输入列像素低八位
#define FPGA_LED_PATTERN_REG        0x11    //FPGA LED闪烁样式控制寄存器
#define FPGA_INTER_CLEAR_REG        0x12    //FPGA 中断清零寄存器, 写1清零
#define FPGA_LANE_COUNT_REG         0x13    //FPGA 当前使用的lane的数量

/**
 * FPGA寄存器掩码定义
 * */
#define FPGA_VGA_IN_BIT (1 << 0)    //该位置一代表VGA接口有视频输入
#define FPGA_SDI_IN_BIT (1 << 4)    //该位置一代表SDI接口有视频输入

#define FPGA_SWITCH_VGA_IN 0x01     //向FPGA_VIDEO_IN_SWITCH_REG寄存器写该值选择VGA输入
#define FPGA_SWITCH_SDI_IN 0x02     //向FPGA_VIDEO_IN_SWITCH_REG寄存器写该值选择SDI输入

#define FPGA_INTER_SDI_C_ABNOR_BIT              (1 << 1)    //中断状态寄存器中SDI输入色度异常标志位
#define FPGA_INTER_SDI_Y_ABNOR_BIT              (1 << 2)    //中断状态寄存器中SDI输入亮度异常标志位
#define FPGA_INTER_VGA_RESOLUTION_CHANGE_BIT    (1 << 3)    //中断状态寄存器中VGA分辨率改变中断标志位
#define FPGA_INTER_SDI_RESOLUTION_CHANGE_BIT    (1 << 4)    //中断状态寄存器中SDI分辨率改变中断标志位
#define FPGA_INTRT_VGA_HOTPLUG_BIT              (1 << 5)    //中断状态寄存器中VGA拔插中断标志位
#define FPGA_INTER_SDI_HOTPLUG_BIT              (1 << 6)    //中断状态寄存器中SDI拔插中断标志位
#define FPGA_INTER_POE                          (1 << 7)    //POE中断

/**
 * 每种颜色的led由两个位控制闪烁样式
 * 如绿色LED由 bit0、bit1控制
 * 红色LED由 bit2、bit3控制
 * */
#define FPGA_SYS_LED_PATTERN_SHIFT   0  //系统指示LED显示样式移位量(对应控制LED面板的绿色LED)
#define FPGA_SLEEP_LED_PATTERN_SHIFT 2  //休眠指示LED显示样式移位量(对应控制LED面板的红色LED)

#define FPGA_LED_PATTERN_MASK (0b11)    //
#define FPGA_LED_PATTERN_ON 0x02        //LED常亮
#define FPGA_LED_PATTERN_OFF 0x00       //LED熄灭
#define FPGA_LED_PATTERN_FLASH 0x01     //LED闪烁

/**
 * 事件标志掩码寄存器
 * */
#define FPGA_EVENT_VGA_RESOLUTION_CHANGE (1 << 0)   //VGA接口有分辨率变更事件
#define FPGA_EVENT_SDI_RESOLUTION_CHANGE (1 << 1)   //SDI接口有分辨率变更事件
#define FPGA_EVENT_VGA_HOTPLUG (1 << 2)     //VGA接口有拔插事件
#define FPGA_EVENT_SDI_HOTPLUG (1 << 3)     //VGA接口有拔插事件
#define FPGA_EVENT_ABNORMAL (1 << 4)        //有异常事件发生
#define FPGA_VIDEO_IN_CHANGED (1 << 5)      //输入视频源发生切换

/**
 * proc命令格式:cmd arg1 arg2
 * */
#define FPGA_PROC_CMD_FMT "%s %s %s"
#define FPGA_PROC_CMD_SET_LED "set_led"
#define FPGA_PROC_CMD_SET_VI "set_vi"

/**
 * ioctl命令定义
 * */
#define FPGA_IOCTL_CMD_GET_STATISTIC    _IO(0x07, 0x01)     //获取FPGA当前的状态信息
#define FPGA_IOCTL_CMD_SET_LED          _IO(0x07, 0x02)     //设置LED显示状态
#define FPGA_IOCTL_CMD_UPDATE           _IO(0x07, 0x03)     //升级FPGA程序
#define FPGA_IOCTL_CMD_SET_VIDEO_SWITCH _IO(0x07, 0x04)     //设置当前视频源
#define FPGA_IOCTL_CMD_GET_BIN          _IO(0x07, 0x05)     //回读flash中的fpga程序

#define FPGA_PROC_MSG_BUFF_SIZE (8 * 1024)                  //提供给驱动写日志信息到proc文件系统的buff大小
#define FPGA_FLITER_MAGIC 10    //拔插消抖时 读取状态的次数

/**
 * 定义netlink消息内容
 * */
#define FPGA_NET_LINK_MSG "fpga_monitor status changed"

typedef struct update_package {
    char* data;
    int length;
} update_package_t;

typedef struct vga_info {
    unsigned int in;                //标志VGA接口是否有视频源输入

    unsigned int frame_rate;        //VGA接口输入的视频源的帧率
    unsigned int resolution_width;  //VGA接口输入的视频源的横向分辨率
    unsigned int resolution_height; //VGA接口输入的视频源的纵向分辨率
} vga_info_t;

typedef struct sdi_info {
    unsigned int in;                //标志SDI接口是否有视频源输入

    unsigned int frame_rate;        //SDI接口输入的视频源的帧率
    unsigned int resolution_width;  //SDI接口输入的视频源的横向分辨率
    unsigned int resolution_height; //SDI接口输入的视频源的纵向分辨率
} sdi_info_t;

typedef struct fpga_statistic {
    unsigned char version;          //fpga版本号

    unsigned char video_switch;     //视频输入源切换开关
    unsigned char led_pattern;      //LED显示样式

    unsigned char event_bits;       //标志当前发生的事件, 应用层可以根据这里边的标志判断发生了什么事件,
                                    //然后做相应的处理

    unsigned char lane_count;       //当前用于传输视频的lane总数

    unsigned int interr_cnt;        //中断计数
    unsigned int sdi_y_abnor_cnt;   //SDI Y分量异常中断计数
    unsigned int sdi_c_abnor_cnt;   //SDI C分量异常中断计数

    vga_info_t vga_info;            //vga输入源信息
    sdi_info_t sdi_info;            //sdi输入源信息
} fpga_statistic_t;

typedef struct fpga_monitor {
    unsigned int irq;               //GPIO中断号

    struct i2c_adapter* adapter;    //fpga所在的i2c总线适配器
    struct i2c_client* client;      //用于和fpga通信

    int intr_pin;                   //fpga所连接的中断引脚
    int start_up_pin;               //fpga启动成功后该引脚是高, 没启动成功时该引脚是0
    int flash_ntf_pin;              //写完flash时控制该引脚拉低200ms并拉高

    struct spi_device* spi_dev;     //用于操作flash的spi dev
} fpga_monitor_t;

#endif
