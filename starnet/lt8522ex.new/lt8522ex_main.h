#ifndef __LT8522EX_H
#define __LT8522EX_H

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/netlink.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/gpio.h>
#include <linux/switch.h>

#include <net/netlink.h>
#include <net/net_namespace.h>
#include <net/sock.h>

#include <asm/io.h>

#include "vga.h"

#define LT8522EX_DEBUG 1

#define lt8522ex_log(fmt, args...) printk("[LT8522EX][FUNC:%s][LINE:%d] "fmt, __FUNCTION__, __LINE__, ##args)
#if LT8522EX_DEBUG
#define lt8522ex_dbg(fmt, args...) printk("[LT8522EX][FUNC:%s][LINE:%d] "fmt, __FUNCTION__, __LINE__, ##args)
#else
lt8522ex_dbg(fmt, args...)
#endif

#define MAX_LT8522EX_CNT 8
#define LT8522EX_NL_MSG_FMT "lt8522ex[%s] status change"

#define SWITCH_NAME_BUFF_SIZE 64

/**
 * 定义ioctl命令
 * */
#define CMD_GET_VGA_OUTPUT_EDID     _IO(0x06, 0x01) //获取VGA输出口所接显示器的EDID
#define CMD_GET_HDMI_OUTPUT_EDID    _IO(0x06, 0x02) //获取HDMI输出口所接显示器的EDID
#define CMD_SET_VGA_INPUT_EDID      _IO(0x06, 0x03) //设置VGA输入口的EDID
#define CMD_SET_HDMI_INPUT_EDID     _IO(0x06, 0x04) //设置HDMI输入口的EDID
#define CMD_GET_LT8522EX_EVENT      _IO(0x06, 0x05) //获取当前发生的事件(拔插事件)
#define CMD_SET_INPUT_OUTPUT_PAIR   _IO(0x06, 0x06) //配置LT8522EX的输入输出关系(例: VGA输入->HDMI输出)

/**
 * 定义LT8522EX上发生的事件掩码
 * */
#define EVENT_HDMI_IN_PLUG_IN   (1 << 0)
#define EVENT_HDMI_IN_PLUG_OUT  (1 << 1)
#define EVENT_VGA_IN_PLUG_IN    (1 << 2)
#define EVENT_VGA_IN_PLUG_OUT   (1 << 3)
#define EVENT_HDMI_OUT_PLUG_IN  (1 << 4)
#define EVENT_HDMI_OUT_PLUG_OUT (1 << 5)
#define EVENT_VGA_OUT_PLUG_IN   (1 << 6)
#define EVENT_VGA_OUT_PLUG_OUT  (1 << 7)

/**
 * 低四位用于选择输入源，高四位选择输出端口
 * 例: LT8522EX_HDMI_INPUT | LT8522EX_VGA_OUTPUT表示当前LT8522以
 * HDMI作为输入源, VGA作为输出端口
 * */
#define LT8522EX_INPUT_MASK     0x0f
#define LT8522EX_OUTPUT_MASK    0xf0
#define LT8522EX_HDMI_INPUT     (1 << 0)
#define LT8522EX_VGA_INPUT      (1 << 1)
#define LT8522EX_HDMI_OUTPUT    (1 << 4)
#define LT8522EX_VGA_OUTPUT     (1 << 5)

/**
 * lt8522ex_t对象没有使用到gpio时, 对应的引脚号初始化为INVALID_GPIO
 * */
#define INVALID_GPIO 0xff
/**
 * 用于指定描述LT8522的输入输出情况
 * */
typedef unsigned char input_output_pair_t;
/**
 * 用于表示一个GPIO引脚
 * */
typedef unsigned int gpio_t;
/**
 * ioctl传递的输入输出配置参数
 * */
typedef struct {
    input_output_pair_t pair1;
    input_output_pair_t pair2;
} input_output_pair_config_t;

/**
 * 一个lt8522ex的抽象
 * */
typedef struct lt8522ex {
    dev_t dev;                  //字符设备号
    char* name;                 //定义这片LT8522EX的名字

    gpio_t hdmi_in_detect_pin;  //用于检测HDMI IN接口18号引脚状态的GPIO. 1:拔出  0:接入
    gpio_t hdmi_in_hpd_pin;     //控制HDMI IN接口的19号引脚, 用于向源端回复HPD信号. 1:HPD信号拉高  0:HPD信号拉低
    gpio_t vga_in_detect_pin;   //用于检测VGA IN接口9号引脚的状态的GPIO   1:拔出  0:接入
    gpio_t vga_out_detect_pin;  //用于检测VGA OUT接口6号引脚的电平状态  1:拔出  0:接入

    gpio_t reset_pin;           //LT8522EX复位引脚
    //LT8522EX有两个输出端口, 所以可以指定两对输入输出对
    //例：pair1 = LT8522EX_HDMI_INPUT | LT8522EX_VGA_OUTPUT
    //   pair2 = LT8522EX_VGA_INPUT | LT8522EX_HDMI_OUTPUT
    input_output_pair_t pair1;  //输入输出对1
    input_output_pair_t pair2;  //输入输出对2
    input_output_pair_t tmp_pair1;      //用户的输入输出配置会被先写到tmp_pairx, 在恰当的时候完成配置更新
    input_output_pair_t tmp_pair2;      //
    int pair_update;                    //这个标记置位意味着lt8522ex的输出、输出配置发生改变, 需要重新配置

    //这个变量记录着lt8522ex发生的事件, 用户通过IOCTL获取并做相应处理
    unsigned int event;                 //这个值在轮询过程中实时更新
    unsigned int event_bak;             //这个值在状态发生变更的时候更新
    struct switch_dev sw_hdmi_in;       //用于在sys文件系统创建来记录lt8522ex HDMI IN端口当前的连接状态
    struct switch_dev sw_hdmi_out;      //用于在sys文件系统创建来记录lt8522ex HDMI OUT端口当前的连接状态
    struct switch_dev sw_vga_in;        //用于在sys文件系统创建来记录lt8522ex VGA IN端口当前的连接状态
    struct switch_dev sw_vga_out;       //用于在sys文件系统创建来记录lt8522ex VGA OUT端口当前的连接状态
/**
 * 注: LT8522EX内部集成多个I2C设备, 所以有若干I2C地址. 不同的
 * 操作所操作的I2C设备地址不同. 所以需要创建多个I2C client.
 * LT8522EX_ADDR_0 0x50
 * LT8522EX_ADDR_1 0x52
 * LT8522EX_ADDR_2 0x54
 * LT8522EX_ADDR_3 0x56
 * */
    struct i2c_adapter* adapter;        //LT8522EX连接的I2C总线所对应的适配器
    struct i2c_client* client_addr0;    //操作addr0时使用的I2C client
    struct i2c_client* client_addr1;    //操作addr0时使用的I2C client
    struct i2c_client* client_addr2;    //操作addr0时使用的I2C client
    struct i2c_client* client_addr3;    //操作addr0时使用的I2C client

    //lt8522ex的状态标记, 轮询时更新状态, 并根据不同状态做相应配置
    char FlagRxHdmi5V;          //该标志置位表示HDMI输入接口的18引脚为5V
    char FlagRxHdmi5VFall;      //该标志置位表示HDMI输入接口的18引脚出现5V-0的下降沿
    char FlagRxHdmi5VRise;      //该标志置位表示HDMI输入接口的18引脚出现0-5V的上升沿
    char FlagTxHdmiStartWork;   //该标志置位表示检测到了HDMI输出口有设备接入，并完成了显示设备的EDID获取
    char FlagTxVgaStartWork;    //该标志置位表示检测到了VGA输出口有设备接入，并完成了显示设备的EDID获取
    char FlagStartWork;
    char FlagTxHpdChange;       //该标志位置位表示HDMI输出口有设备接入
    char FlagVgaDetChange;      //该标志置位表示检测到VGA输出口有设备接入(GPIO_D4 被 VGA 6号脚拉低)
    char FlagTxHdmiOff;         //该标志置位表示检测到HDMI输出口有设备被拔出
    char FlagTxVgaOff;          //该标志置位表示检测到VGA输出口的设备被拔出(GPIO_D4变为高)
    char FlagTxChangeRx;
    char OldTxPlugStatus;
    char OldVgaDetStatus;

    //VGA输入接口相关的属性和状态标志
    char FlagVgaTimingChange;   //该标志置位表示VGA输入时序发生了改变
    char AutoPhaseDone;
    VESA_Resolution videoformat;
    VESA_Resolution LastVideoFormat;

    unsigned char HdmiSinkEdid[256];    //用于存放HDMI输出接口外接显示器的EDID
    int hdmi_in_edid_update;            //该标志置位表示需要更新hdmi in接口的EDID
    unsigned char VgaSinkEdid[128];     //用于存放VGA输出接口外接显示器的EDID
    int vga_in_edid_update;             //该标志置位表示需要更新vga in接口的EDID
    unsigned char EdidBuff[256];        //用于存放整合后的EDID

    struct mutex edid_mutex;            //用于控制互斥操作EDID相关的资源
} lt8522ex_t;

#endif
