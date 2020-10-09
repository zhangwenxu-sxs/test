#ifndef __LT6911UXC_H
#define __LT6911UXC_H

#define LT6911UXC_DEBUG 1

#define USE_INTERRUPT 1                 //采用中断方式触发更新lt6911uxc状态
#define USE_POLL 2                      //采用轮询方式触发更新lt6911uxc状态
#define MAX_LT6911UXC_CNT 8             //最大支持8个lt6911uxc

//#define DETECT_POLICY USE_POLL        //默认采用轮询方式来检测lt6911uxc的状态变更
#define DETECT_POLICY USE_INTERRUPT     //默认采用中断方式来检测lt6911uxc的状态变更

//定义用于通知应用的netlink消息内容格式
#define LT6911UXC_NL_MSG_FMT "which:%s  event:%s"
#define LT6911UXC_PLUG_IN_STR "plug_in"
#define LT6911UXC_PLUG_OUT_STR "plug_out"

//定义日志格式
#define lt6911uxc_log(fmt, args...) printk("[LT6911UXC][FUNC:%s][LINE:%d] "fmt, __FUNCTION__, __LINE__, ##args)
#if LT6911UXC_DEBUG
#define lt6911uxc_dbg(fmt, args...) printk("[LT6911UXC][FUNC:%s][LINE:%d] "fmt, __FUNCTION__, __LINE__, ##args)
#else
#define lt6911uxc_dbg(fmt, args...)
#endif

//定义ioctl命令
#define CMD_LT6911UXC_STATUS_GET    _IO(0x05, 0x01)
#define CMD_LT6911UXC_RESET         _IO(0x05, 0x02)
#define CMD_LT6911UXC_MUTE          _IO(0x05, 0x03)
#define CMD_LT6911UXC_UNMUTE        _IO(0x05, 0x04)
//定义lt6911uxc事件的类型
#define EVENT_HDMI_PLUG_IN      (1 << 0)    //HDMI接入
#define EVENT_HDMI_PLUG_OUT     (1 << 1)    //HDMI拔出
#define EVENT_HDMI_ERROR        (1 << 7)    //HDMI出现了某些未知错误

//枚举颜色空间类型
typedef enum color_space {
    COLOR_SPACE_RGB888 = 0,
    COLOR_SPACE_YUV444 = 1,
    COLOR_SPACE_YUV422 = 2,
    COLOR_SPACE_YUV420 = 3
} color_space_e;

//一次hdmi_event事件的抽象
typedef struct hdmi_event {
    int hactive;            //水平方向的分辨率
    int vactive;            //竖直方向的分辨率
    int framerate;          //帧率

    int samplerate;         //音频采样率
    int have_audio;         //是否有音频输入

    int lane_count;         //mipi csi有多少条lane有数据输出

    int event_flag;         //用于标记哪些状态发生了改变
} hdmi_event_t;

//一片lt6911uxc的抽象
typedef struct lt6911uxc {
    dev_t dev;                          //维护lt6911uxc的设备号
#if DETECT_POLICY == USE_INTERRUPT
    unsigned int irq;                   //当这个中断触发时更新lt6911uxc的状态
#endif
    struct i2c_adapter* adapter;
    struct i2c_client* client;          //用于和lt6911uxc进行i2c通信

    int reset_pin;                      //lt6911uxc复位引脚
    int interrupt_pin;                  //lt6911uxc使用的中断管脚的管脚号

    char* name;                         //这片lt6911uxc的名字

    struct switch_dev sw;               //用于在sys文件系统创建来记录当前的连接状态
    hdmi_event_t hdmi_event;            //表征这块lt6911uxc上发生的事件
} lt6911uxc_t;

#endif
