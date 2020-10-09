#ifndef __DEVICE_DETECT_H
#define __DEVICE_DETECT_H

#include <linux/printk.h>
#include <linux/interrupt.h>

#define sys_debug_log(fmt, args...) printk("[SENSOR_HUB_GPIO][FUNC:%s][LINE:%d] "fmt, __FUNCTION__, __LINE__, ##args)

/**
 * 一些寄存器的基址定义
 * */
#define HI3559_GPIO_CFG_REG_BASE 0x18050000         //管脚配置寄存器基址

#define HI3559_GPIO0_REG_BASE 0x180d0000            //GPIO0 寄存器基址
#define HI3559_GPIO1_REG_BASE 0x180d1000            //GPIO1 寄存器基址
#define HI3559_GPIO2_REG_BASE 0x180d2000            //GPIO2 寄存器基址
#define HI3559_GPIO3_REG_BASE 0x180d3000            //GPIO3 寄存器基址
#define HI3559_GPIO4_REG_BASE 0x180d4000            //GPIO4 寄存器基址

/**
 * GPIO配置寄存器位掩码定义
 * */
#define FORCE_PULL_UP_MASK (1 << 14)                //强上拉使能位(1有效)
#define INPUT_ENABLE_MASK (1 << 12)                 //输入使能位(1有效)
#define SMIT_INPUT_ENABLE_MASK (1 << 11)            //斯密特输入使能位(1有效)
#define SPEED_CONTROL_MASK (1 << 10)                //电平转换速度控制寄存器(1:电平转换速度慢  0:电平转换速度快)
#define PULL_DOWN_ENABLE_MASK (1 << 9)              //下拉电阻使能(1有效)
#define PULL_UP_ENABLE_MASK (1 << 8)                //上拉电阻使能(1有效)
#define DRIVE_ABILITY_MASK (0b1111 << 4)            //驱动能力控制位(档位1~16)
#define IO_FUNCTION_MASK (0b1111 << 0)              //引脚功能控制寄存器(控制GPIO复用情况)

/**
 * 用于操作GPIO配置寄存器的一些宏
 * */

/**
 * 使能GPIO强上拉
 * */
#define set_force_pull_up_enable(gpio) do {                                         \
                                (*gpio->config_register) |= (FORCE_PULL_UP_MASK);   \
                                } while(0)
/**
 * 禁用GPIO强上拉
 * */
#define set_force_pull_up_disable(gpio) do {                                        \
                                (*gpio->config_register) &= (~FORCE_PULL_UP_MASK);  \
                                } while(0)

/**
 * 使能GPIO输入
 * */
#define set_input_enable(gpio) do {                                                 \
                            (*gpio->config_register) |= (INPUT_ENABLE_MASK);        \
                            } while(0)

/**
 * 禁用GPIO输入
 * */
#define set_input_disable(gpio) do {                                                \
                            (*gpio->config_register) &= (~INPUT_ENABLE_MASK);       \
                            } while(0)

/**
 * 使能GPIO斯密特输入
 * */
#define set_smit_input_enable(gpio) do {                                            \
                            (*gpio->config_register) |= (SMIT_INPUT_ENABLE_MASK);   \
                            } while(0)

/**
 * 禁用GPIO斯密特输入
 * */
#define set_smit_input_disable(gpio) do {                                           \
                            (*gpio->config_register) &= (~SMIT_INPUT_ENABLE_MASK);  \
                            } while(0)

/**
 * 设置GPIO电平转换速度为快
 * */
#define set_gpio_quick(gpio) do {                                                   \
                            (*gpio->config_register) &= (~SPEED_CONTROL_MASK);      \
                            } while(0)

/**
 * 设置GPIO电平转换速度为慢
 * */
#define set_gpio_slow(gpio) do {                                                    \
                            (*gpio->config_register) |= (SPEED_CONTROL_MASK);       \
                            } while(0)

/**
 * 设置GPIO下拉电阻使能
 * */
#define set_pull_down_enable(gpio) do {                                             \
                            (*gpio->config_register) |= (PULL_DOWN_ENABLE_MASK);    \
                            } while(0)

/**
 * 禁用GPIO下拉电阻
 * */
#define set_pull_down_disable(gpio) do {                                            \
                            (*gpio->config_register) &= (~PULL_DOWN_ENABLE_MASK);   \
                            } while(0)

/**
 * 设置GPIO上拉电阻使能
 * */
#define set_pull_up_enable(gpio) do {                                               \
                            (*gpio->config_register) |= (PULL_UP_ENABLE_MASK);      \
                            } while(0)

/**
 * 禁用GPIO上拉电阻
 * */
#define set_pull_up_disable(gpio) do {                                              \
                            (*gpio->config_register) &= (~PULL_UP_ENABLE_MASK);     \
                            } while(0)

/**
 * 设置GPIO驱动能力等级(0~15)
 * */
#define set_drive_level(gpio, level) do {                                           \
                        (*gpio->config_register) &= (~DRIVE_ABILITY_MASK);          \
                        (*gpio->config_register) |= (((level << 4) & DRIVE_ABILITY_MASK));  \
                        } while(0)

/**
 * 将管脚的复用情况设置为通用GPIO
 * */
#define set_pin_as_gpio(gpio) do {                                                  \
                        (*gpio->config_register) &= (~IO_FUNCTION_MASK);            \
                        } while(0)

/**
 * 3559 的GPIO组的枚举
 * */
typedef enum {
    HI3559_GPIO_GROUP0 = 0,
    HI3559_GPIO_GROUP1 = 1,
    HI3559_GPIO_GROUP2 = 2,
    HI3559_GPIO_GROUP3 = 3,
    HI3559_GPIO_GROUP4 = 4,
} hi3559_gpio_group_e;

/**
 * 3559 GPIO 引脚枚举
 *   GPIO4只有两个引脚
 * */
typedef enum {
    HI3559_GPIO_PIN0 = 0,
    HI3559_GPIO_PIN1 = 1,
    HI3559_GPIO_PIN2 = 2,
    HI3559_GPIO_PIN3 = 3,
    HI3559_GPIO_PIN4 = 4,
    HI3559_GPIO_PIN5 = 5,
    HI3559_GPIO_PIN6 = 6,
    HI3559_GPIO_PIN7 = 7
} hi3559_gpio_pin_e;

/**
 * 枚举GPIO所支持的一些中断类型
 * LEVEL_ 前缀表示电平触发
 * EDGE_ 前缀表示边沿触发
 * */
typedef enum {
    LEVEL_HIGH = 0,     //高电平触发
    LEVEL_LOW = 1,      //低电平触发

    EDGE_RISING = 2,    //上升沿触发
    EDGE_FALLING = 3,   //下降沿触发
    EDGE_BILATERAL = 4  //双边沿触发
} hi3559_gpio_intr_type_e;

/**
 * 枚举GPIO输出方向
 * */
typedef enum {
    GPIO_DIR_OUTPUT = 0,    //GPIO设置为输出
    GPIO_DIR_INPUT = 1      //GPIO设置为输入
} hi3559_gpio_dir_e;

/**
 * 枚举GPIO当前的用途
 * */
typedef enum {
    GPIO_USE_UNKNOWN = 0,   //还处于未知用途
    GPIO_USE_OUTPUT = 1,    //用于通用输出端口
    GPIO_USE_INPUT = 2,     //用于通用输入端口
    GPIO_USE_INTERRUPT = 3  //用于中断输入端口
} hi3559_gpio_use_e;

/**
 * 一组GPIO控制寄存器的抽象
 * */
typedef struct {
    unsigned int GPIO_DIR;      //GPIO方向控制寄存器
    unsigned int GPIO_IS;       //GPIO中断触发寄存器
    unsigned int GPIO_IBE;      //GPIO双边沿触发中断寄存器
    unsigned int GPIO_IEV;      //GPIO触发中断条件寄存器
    unsigned int GPIO_IE;       //GPIO中断屏蔽寄存器
    unsigned int GPIO_RIS;      //GPIO原始中断状态寄存器
    unsigned int GPIO_MIS;      //GPIO中断屏蔽状态寄存器
    unsigned int GPIO_IC;       //GPIO中断清除寄存器
} hi3559_gpio_ctl_reg_t;

/**
 * 一个GPIO管脚的抽象
 * */
typedef struct {
    unsigned int gpio_group;                        //属于哪个GPIO组
    unsigned int gpio_pin;                          //属于哪个GPIO管脚, 每个GPIO 组有0-7 共8根引脚

    unsigned int irq_num;                           //该GPIO的中断号

    volatile unsigned char* base_address;           //该组GPIO的寄存器基址
    volatile unsigned int* data_register;           //指向操作该GPIO的数据寄存器
    volatile unsigned int* config_register;         //GPIO配置寄存器

    volatile hi3559_gpio_ctl_reg_t* gpio_ctl_reg;   //用于控制GPIO的寄存器的集合

    char name[32];                                  //名字
    hi3559_gpio_use_e curr_use;                     //表示该GPIO当前的用途
} hi3559_gpio_t;

//创建并初始化一个GPIO引脚, 后续对该GPIO的操作基于该函数返回的数据结构进行
hi3559_gpio_t* create_gpio(hi3559_gpio_group_e group, hi3559_gpio_pin_e pin);
//销毁并释放GPIO所用到的一些资源
int destroy_gpio(hi3559_gpio_t* gpio);

//设置GPIO中断
int gpio_intr_register(hi3559_gpio_t* gpio, hi3559_gpio_intr_type_e intr_type, irq_handler_t handler);
void gpio_intr_unregister(hi3559_gpio_t* gpio);
//控制GPIO输出值 value取值: 0 低电平   1 高电平
void set_gpio_value(hi3559_gpio_t* gpio, int value);
//读取GPIO上当前的电平状态  返回值: 0 低电平    1 高电平
int get_gpio_value(hi3559_gpio_t* gpio);

#endif
