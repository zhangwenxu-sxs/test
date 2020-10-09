#ifndef __OLED_SSD1322_H
#define __OLED_SSD1322_H

#include <linux/nls.h>
#include <linux/mutex.h>

#define OLED_DEBUG 0

#define oled_log(fmt, args...) printk("[OLED][FUNC:%s][LINE:%d] "fmt, __FUNCTION__, __LINE__, ##args)

#if OLED_DEBUG
#define oled_dbg(fmt, args...) printk("[OLED][FUNC:%s][LINE:%d] "fmt, __FUNCTION__, __LINE__, ##args)
#else
#define oled_dbg(fmt, args...)
#endif

#define OLED_ROW_NUM 64     //oled行数
#define OLED_COL_NUM 256    //oled列数
#define OLED_COLOR_DEPTH 4  //这块oled的颜色深度是4，即用4位数据控制

#define CMD_OLED_POWER_ON       _IO(0x07, 0x01)         //控制oled做上电复位时序
#define CMD_OLED_POWER_OFF      _IO(0x07, 0x02)         //控制oled做下电时序
#define CMD_OLED_SET_DISP_MODE  _IO(0x07, 0x03)         //配置oled的显示模式
#define CMD_OLED_RENDER_CHAR    _IO(0x07, 0x04)         //渲染一个unicode字符到指定framebuffer
#define CMD_OLED_CLEAR_FB       _IO(0x07, 0x05)         //清零oled的framebuffer
#define CMD_OLED_FLUSH_FB       _IO(0x07, 0x06)         //将framebuffer中的数据更新到oled显示
#define CMD_OLED_SET_FONT_LIB   _IO(0x07, 0x07)         //配置oled所使用的unicode点阵字库
#define CMD_OLED_GET_OLED_INFO  _IO(0x07, 0x08)         //用于获取oled的参数(分辨率、颜色深度)
#define CMD_OLED_SET_DISP_ANIMATION  _IO(0x07, 0x09)    //配置oled显示内容时的动画效果
#define CMD_OLED_SET_SWITCH_ANIMATION  _IO(0x07, 0x0a)  //配置oled刷新内容时的切换动画

/**
 * 枚举可能的传输类型
 * */
typedef enum {
    OLED_TX_DATA = 0,
    OLED_TX_COMMAND = 1
} oled_tx_type_e;

/**
 * 枚举oled显示内容时的动画效果
 * */
typedef enum {
    //显示动画效果
    OLED_ANIMATION_STATIC = 0,      //静态显示
    OLED_ANIMATION_ROLL = 1         //滚动显示
} oled_disp_animation_e;

/**
 * 枚举oled刷新显示内容时的刷新动画
 * */
typedef enum {
    //切换动画效果
    OLED_ANIMATION_NONE = 0,        //没有动画效果
    OLED_ANIMATION_LEFT_IN = 1,     //从左滑入
    OLED_ANIMATION_RIGHT_IN = 2,    //从右滑入
    OLED_ANIMATION_TOP_IN = 3,      //从上边滑入
    OLED_ANIMATION_BOTTM_IN = 4,    //从下边滑入
} oled_switch_animation_e;

/**
 * 枚举oled可能的显示模式
 * */
typedef enum {
    OLED_DISP_NORMAL = 0,       //正常显示模式，显示GDDRAM中的数据
    OLED_DISP_ON = 1,           //全亮模式，不管GDDRAM中的数据是怎么样的，oled的所有像素都点亮
    OLED_DISP_OFF = 2,          //全灭模式，不管GDDRAM中的数据是怎么样的，oled的所有像素都熄灭
    OLED_DISP_INVERSE = 3       //翻转模式，GDDRAM中指定的亮的像素灭掉，GDDRAM中指定灭掉的像素亮起
} oled_disp_mode_e;

/**
 * 调用CMD_OLED_SET_FONT_LIB命令时传的数据结构
 * */
typedef struct font_lib_cfg {
    unsigned char* font_lib;    //字库所在的内存
    unsigned int length;        //字库所占的内存大小
} font_lib_config_t;

/**
 * 调用CMD_OLED_RENDER_CHAR命令时传的数据结构
 * */
typedef struct render_char {
    int x_pos;      //显示的x轴坐标
    int y_pos;      //显示的y轴坐标
    unicode_t ch[64];   //要显示在oled上的unicode字符串(一个屏幕最多显示64个字符)
    unsigned int cnt;   //字符里边的字符个数
} render_char_t;

/**
 * 调用CMD_OLED_GET_OLED_INFO命令时返回的数据结构，用于描述oled的像素点数
 * */
typedef struct oled_info {
    unsigned int x_res;     //水平方向分辨率
    unsigned int y_res;     //垂直方向分辨率
    unsigned char depth;    //颜色深度，如4bits、8bits、10bits
    unsigned int fb_size;   //使用的framebuffer的大小
} oled_info_t;

/**
 * 描述一次spi传输
 * */
typedef struct oled_transfer {
    unsigned char data;
    oled_tx_type_e type;
} oled_transfer_t;

/**
 * 一块ssd1322 oled模组的抽象
 * */
typedef struct oled_ssd1322 {
    dev_t cdev;                     //字符设备号
    struct spi_device* spi_dev;     //用于操作oled的SPI设备

    unsigned int speed_hz;          //spi时钟频率

    unsigned int reset_pin;         //复位引脚
    unsigned int power_pin;         //电源控制脚
    unsigned int cmd_data_pin;      //用于控制当前发送的是数据还是命令的引脚

    unsigned char framebuffer[OLED_ROW_NUM][OLED_COL_NUM / 2];  //oled的显存

    unsigned char* font_lib;        //用于存储点阵字库的缓冲区

    struct mutex tx_mutex;          //用于保证通过spi往oled写命令和数据时同步
    struct mutex fb_mutex;          //用于保护framebuffer的互斥锁

    int power_on;                   //标志oled是否完成上电时序

    oled_disp_animation_e disp_animation;       //显示动画效果
    oled_switch_animation_e switch_animation;   //切换动画效果
} oled_ssd1322_t;

#endif
