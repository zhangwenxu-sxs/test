#ifndef __GPIO_CTRL_H
#define __GPIO_CTRL_H

#include <linux/list.h>
#include <linux/mutex.h>

#define GPIO_CTRL_DBG 1

#define ACTION_ATTR_PREFIX  "action"
#define OPERATION_ATTR_PREFIX "operation"

#define SHUB_GPIO_OFFSET 152        //gpio号从152开始表示SHUB_GPIO, 如152表示SHUB_GPIO0_0

#define CHIP_MAX_GPIO_CNT   32      //芯片最多能够使用的GPIO数
#define PROC_MSG_SIZE (8 * 1024)    //proc文件节点最大能read出来的字节数

#define gpio_hub_log(fmt, args...) printk("[GPIO_CTRL_HUB][FUNC:%s][LINE:%d] "fmt, __FUNCTION__, __LINE__, ##args)

#if GPIO_CTRL_DBG
#define gpio_hub_dbg(fmt, args...) printk("[GPIO_CTRL_HUB][FUNC:%s][LINE:%d] "fmt, __FUNCTION__, __LINE__, ##args)
#else
#define gpio_hub_dbg(fmt, args...)
#endif

/**
 * 一次写寄存器操作、或是延时操作的抽象, 抽象成一个action
 * */
typedef struct action {
    struct list_head action_node;   //链表节点

    int gpio;       //这次action操作的GPIO
    int val;        //这次设到GPIO的值
    int delay_us;   //昨晚GPIO操作后延时的时间
} action_t;

/**
 * 一个芯片所包含的操作的抽象
 * */
typedef struct operation {
    struct list_head operation_node;//链表节点

    char* name;                     //operation的名字, 如power_on、power_off、reset之类的
    int should_do;                  //这个operation是否需要在加载驱动时执行
    struct list_head actions;       //完成一次上电操作所包含的action
} operation_t;

/**
 * 一片芯片的抽象, 它包含了用于操作它的chip_operation_t,
 * 以及芯片所接的复位控制引脚和电源控制引脚
 * */
typedef struct chip {
    struct list_head chip_node;     //链表节点

    struct mutex chip_mutex;        //用于确保同时只有一个进程操作chip, 防止时序乱掉

    int need_proc;                  //是否需要在/proc/starnet/gpio_ctrl_hub下创建文件节点
    int gpios[CHIP_MAX_GPIO_CNT];   //该芯片所使用的GPIO组成的数组

    char* name;                     //芯片名字, /proc/starnet/gpio_ctrl_hub目录下会创建同名节点
    struct list_head operations;    //芯片所包含的操作的集合

    char proc_buffer[PROC_MSG_SIZE];    //cat /proc/gpio_ctrl_hub/xxxxx打印出来的信息
    int buffer_idx;                 //用于遍历proc_buffer的索引
} chip_t;

#endif
