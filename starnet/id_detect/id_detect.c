/**
 * 该驱动的作用: 读取硬件版本号、板卡ID、CPU ID并上报给应用
 *
 * 注：硬件版本号、ID通过读取GPIO的电平状态获得
 * */

#include <linux/printk.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/switch.h>
#include <linux/kthread.h>

#include <asm/io.h>

#include "sensor_hub_gpio.h"

#define SN_TEST 0
#define id_detect_log(fmt, args...) printk("[ID_DETECT][FUNC:%s][LINE:%d] "fmt, __FUNCTION__, __LINE__, ##args)

/**
 * 用于向上层应用上报硬件版本号
 * */
static struct switch_dev hw_version = {
    .name = "hw_version"
};

/**
 * 用于向上层应用上报cpu id
 * */
static struct switch_dev cpu_id = {
    .name = "cpu_id"
};

/**
 * 用于向上层应用上报board id
 * */
static struct switch_dev board_id = {
    .name = "board_id"
};

/**
 * 用于存储ID、硬件版本信息的结构
 * */
static struct {
    int cpu_id;       //2位的CPU ID
    int hw_version;   //6位的硬件版本号
    int board_id;     //4位的槽位ID
} id_info;

/**
 * 一个用于测试用的线程函数
 * */
#if SN_TEST
int test_thread(void* data)
{
    int val = 0;
    hi3559_gpio_t* gpio3_7;
    gpio3_7 = create_gpio(3, 7);

    while(1) {
        val = get_gpio_value(gpio3_7);
        id_detect_log("%s:%d \n", gpio3_7->name, val);

        usleep_range(2000000, 2000100);
    }

    return 0;
}
#endif

static int id_detect_init(void)
{
    int val = 0;    //用于暂存读取出来的GPIO电平状态

    //用于读取硬件版本ID的GPIO
    hi3559_gpio_t* gpio0_2;
    hi3559_gpio_t* gpio0_3;
    hi3559_gpio_t* gpio1_0;
    hi3559_gpio_t* gpio1_1;
    hi3559_gpio_t* gpio1_2;
    hi3559_gpio_t* gpio1_3;

    //用于读取CPU ID的GPIO
    hi3559_gpio_t* gpio0_0;
    hi3559_gpio_t* gpio0_1;

    //用于读取卡槽ID的GPIO
    hi3559_gpio_t* gpio0_4;
    hi3559_gpio_t* gpio0_5;
    hi3559_gpio_t* gpio0_6;
    hi3559_gpio_t* gpio0_7;

    //创建用于读取硬件版本号的GPIO实例
    gpio0_2 = create_gpio(0, 2);
    gpio0_3 = create_gpio(0, 3);
    gpio1_0 = create_gpio(1, 0);
    gpio1_1 = create_gpio(1, 1);
    gpio1_2 = create_gpio(1, 2);
    gpio1_3 = create_gpio(1, 3);

    //创建用于读取CPU ID的GPIO实例
    gpio0_0 = create_gpio(0, 0);
    gpio0_1 = create_gpio(0, 1);

    //创建用于读取卡槽ID的GPIO实例
    gpio0_4 = create_gpio(0, 4);
    gpio0_5 = create_gpio(0, 5);
    gpio0_6 = create_gpio(0, 6);
    gpio0_7 = create_gpio(0, 7);

    //清零ID、版本号
    id_info.board_id = 0;
    id_info.cpu_id = 0;
    id_info.hw_version = 0;

    //注册switch设备用于向应用上报版本号、ID信息
    switch_dev_register(&hw_version);
    switch_dev_register(&cpu_id);
    switch_dev_register(&board_id);

    //读取GPIO的电平状态, 确定ID、版本号信息
    val = get_gpio_value(gpio0_2);
    id_info.hw_version |= (val << 0);
    val = get_gpio_value(gpio0_3);
    id_info.hw_version |= (val << 1);
    val = get_gpio_value(gpio1_0);
    id_info.hw_version |= (val << 2);
    val = get_gpio_value(gpio1_1);
    id_info.hw_version |= (val << 3);
    val = get_gpio_value(gpio1_2);
    id_info.hw_version |= (val << 4);
    val = get_gpio_value(gpio1_3);
    id_info.hw_version |= (val << 5);

    val = get_gpio_value(gpio0_0);
    id_info.cpu_id |= (val << 0);
    val = get_gpio_value(gpio0_1);
    id_info.cpu_id |= (val << 1);

    val = get_gpio_value(gpio0_4);
    id_info.board_id |= (val << 0);
    val = get_gpio_value(gpio0_5);
    id_info.board_id |= (val << 1);
    val = get_gpio_value(gpio0_6);
    id_info.board_id |= (val << 2);
    val = get_gpio_value(gpio0_7);
    id_info.board_id |= (val << 3);

    //将硬件版本号、ID等信息上报给应用
    switch_set_state(&hw_version, id_info.hw_version);
    switch_set_state(&cpu_id, id_info.cpu_id);
    switch_set_state(&board_id, id_info.board_id);

    //读取完后释放相关资源
    destroy_gpio(gpio0_2);
    destroy_gpio(gpio0_3);
    destroy_gpio(gpio1_0);
    destroy_gpio(gpio1_1);
    destroy_gpio(gpio1_2);
    destroy_gpio(gpio1_3);
    destroy_gpio(gpio0_0);
    destroy_gpio(gpio0_1);
    destroy_gpio(gpio0_4);
    destroy_gpio(gpio0_5);
    destroy_gpio(gpio0_6);
    destroy_gpio(gpio0_7);

#if SN_TEST
    kthread_run(test_thread, NULL, "id_detect_test");
#endif

    return 0;
}

static void id_detect_exit(void)
{
    switch_dev_unregister(&hw_version);
    switch_dev_unregister(&cpu_id);
    switch_dev_unregister(&board_id);
}

module_init(id_detect_init);
module_exit(id_detect_exit);

MODULE_AUTHOR("WUJING");
MODULE_DESCRIPTION("ID DETECT DRIVER");
MODULE_LICENSE("GPL");
