/**
 * 该驱动的作用: 1、控制GPIO输出
 *             2、读取GPIO电平状态
 *             3、注册中断处理函数
 * 注：
 *     驱动中只实现了sensor hub的GPIO控制, 通用的GPIO控制海思已经实现. 该
 *     驱动提供了sensor hub gpio的控制接口供其他模块调用.
 * */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <asm/io.h>

#include "sensor_hub_gpio.h"

/**
 * 构建一个hi3559_gpio_t类型的对象, 后续对gpio的操作都通过操作该对象实现.
 * 构建时主要做一些内存映射和初始化寄存器地址之类的操作.
 * */
hi3559_gpio_t* create_gpio(hi3559_gpio_group_e group, hi3559_gpio_pin_e pin)
{
    hi3559_gpio_t* gpio = NULL;

    unsigned int gpio_base_addr_tab[] = {
        HI3559_GPIO0_REG_BASE,
        HI3559_GPIO1_REG_BASE,
        HI3559_GPIO2_REG_BASE,
        HI3559_GPIO3_REG_BASE,
        HI3559_GPIO4_REG_BASE
    };

    //参数检查
    if (group < HI3559_GPIO_GROUP0 || group > HI3559_GPIO_GROUP4 || pin < HI3559_GPIO_PIN0 || pin > HI3559_GPIO_PIN7) {
        sys_debug_log("invalid gpio group or gpio pin \n");
        goto err1;
    }

    //GPIO组4只有两个引脚
    if (group == HI3559_GPIO_GROUP4 && pin > HI3559_GPIO_PIN1) {
        sys_debug_log("invalid gpio group or gpio pin \n");
        goto err1;
    }

    //分配内存
    gpio = (hi3559_gpio_t*)kzalloc(sizeof(hi3559_gpio_t), 0);
    if (gpio == NULL) {
        sys_debug_log("alloc memory for gpio failed \n");
        goto err1;
    }

    //初始化gpio组和引脚号
    gpio->gpio_group = group;
    gpio->gpio_pin = pin;

    //初始化当前gpio的用途为未知状态
    gpio->curr_use = GPIO_USE_UNKNOWN;
    //初始化对象的名字
    sprintf(gpio->name, "SENSOR_HUB_GPIO%u_%u", gpio->gpio_group, gpio->gpio_pin);

    //初始化该gpio的中断号
    gpio->irq_num = 212 + gpio->gpio_group;

    //映射GPIO组的控制寄存器到虚拟地址空间
    gpio->base_address = (unsigned char*)ioremap(gpio_base_addr_tab[gpio->gpio_group], 0x1000);
    if (gpio->base_address == NULL) {
        sys_debug_log("do ioremap(0x%x, 0x1000) failed \n", gpio_base_addr_tab[gpio->gpio_group]);
        goto err2;
    }

    //初始化该引脚对应的数据寄存器地址
    gpio->data_register = (unsigned int*)(gpio->base_address + (1 << (gpio->gpio_pin + 2)));
    gpio->gpio_ctl_reg = (hi3559_gpio_ctl_reg_t*)(gpio->base_address + 0x400);

    //初始化管脚配置寄存器基址
    gpio->config_register = (unsigned int*)ioremap(HI3559_GPIO_CFG_REG_BASE + 32 * gpio->gpio_group + 4 * gpio->gpio_pin, 4);
    if (gpio->config_register == NULL) {
        sys_debug_log("do ioremap(0x%x, 4) failed \n", HI3559_GPIO_CFG_REG_BASE + 32 * gpio->gpio_group + 4 * gpio->gpio_pin);
        goto err3;
    }

    return gpio;

err3:
    iounmap(gpio->base_address);
err2:
    kfree(gpio);
err1:
    return NULL;
}

EXPORT_SYMBOL_GPL(create_gpio);

/**
 * 释放gpio相关资源
 * */
int destroy_gpio(hi3559_gpio_t* gpio)
{
    if (gpio == NULL) {
        sys_debug_log("invalid parameter for destroy_gpio \n");
        return -1;
    }

    if (gpio->curr_use == GPIO_USE_INTERRUPT)
        gpio_intr_unregister(gpio);             //解注册中断处理函数

    if (gpio->base_address != NULL)
        iounmap(gpio->base_address);            //解除内存映射

    if (gpio->config_register != NULL)
        iounmap(gpio->config_register);         //解除内存映射

    kfree(gpio);

    return 0;
}

EXPORT_SYMBOL_GPL(destroy_gpio);

/**
 * 配置引脚为中断引脚并注册中断处理函数
 * */
int gpio_intr_register(hi3559_gpio_t* gpio, hi3559_gpio_intr_type_e intr_type, irq_handler_t handler)
{
    int ret;

    //检查参数
    if (gpio == NULL || intr_type < LEVEL_HIGH || intr_type > EDGE_BILATERAL || handler == NULL) {
        sys_debug_log("invalid parameter for set_gpio_intr \n");
        return -1;
    }

    //将对应的GPIO引脚配置成中断输入引脚
    //第一步: 配置该管脚的配置寄存器, 将管脚复用成通用GPIO
    set_force_pull_up_disable(gpio);
    set_input_enable(gpio);
    set_smit_input_disable(gpio);
    set_gpio_quick(gpio);
    set_pull_down_disable(gpio);
    set_pull_up_disable(gpio);
    set_drive_level(gpio, 0);
    set_pin_as_gpio(gpio);

    //第二步: 配置GPIO为输入
    gpio->gpio_ctl_reg->GPIO_DIR &= (~(1 << gpio->gpio_pin));

    //第三步: 配置中断触发方式
    switch(intr_type) {
    case LEVEL_HIGH:
        gpio->gpio_ctl_reg->GPIO_IS |= (1 << gpio->gpio_pin);       //中断触发方式为电平触发
        gpio->gpio_ctl_reg->GPIO_IEV |= (1 << gpio->gpio_pin);      //中断触发条件是高电平触发
        break;
    case LEVEL_LOW:
        gpio->gpio_ctl_reg->GPIO_IS |= (1 << gpio->gpio_pin);       //中断触发方式为电平触发
        gpio->gpio_ctl_reg->GPIO_IEV &= (~(1 << gpio->gpio_pin));   //中断触发条件是低电平触发
        break;
    case EDGE_RISING:
        gpio->gpio_ctl_reg->GPIO_IS &= (~(1 << gpio->gpio_pin));    //中断触发方式为边沿触发
        gpio->gpio_ctl_reg->GPIO_IBE &= (~(1 << gpio->gpio_pin));   //单边沿触发
        gpio->gpio_ctl_reg->GPIO_IEV |= (1 << gpio->gpio_pin);      //中断触发条件是上升沿触发
        break;
    case EDGE_FALLING:
        gpio->gpio_ctl_reg->GPIO_IS &= (~(1 << gpio->gpio_pin));    //中断触发方式为边沿触发
        gpio->gpio_ctl_reg->GPIO_IBE &= (~(1 << gpio->gpio_pin));   //单边沿触发
        gpio->gpio_ctl_reg->GPIO_IEV &= (~(1 << gpio->gpio_pin));   //中断触发条件是下降沿触发
        break;
    case EDGE_BILATERAL:
        gpio->gpio_ctl_reg->GPIO_IS &= (~(1 << gpio->gpio_pin));    //中断触发方式为边沿触发
        gpio->gpio_ctl_reg->GPIO_IBE |= (1 << gpio->gpio_pin);      //双边沿触发
        break;
    default:
        sys_debug_log("unsupport gpio interrupt type \n");
        return -1;
    }

    //第四步: 清除中断标志
    gpio->gpio_ctl_reg->GPIO_IC |= (1 << gpio->gpio_pin);           //清除中断标志

    //第五步: 注册中断处理函数
    ret = request_irq(gpio->irq_num, handler, IRQF_SHARED, gpio->name, gpio);
    if (ret != 0) {
        sys_debug_log("request irq for %s failed, error code:%d \n", gpio->name, ret);
        return -1;
    }

    //第六步: 使能中断
    gpio->gpio_ctl_reg->GPIO_IE |= (1 << gpio->gpio_pin);           //使能中断

    //第七步: 更新gpio状态
    gpio->curr_use = GPIO_USE_INTERRUPT;                            //表示当前gpio用作中断输入

    return 0;
}

EXPORT_SYMBOL_GPL(gpio_intr_register);

/**
 * 解除注册到内核的中断处理函数
 * */
void gpio_intr_unregister(hi3559_gpio_t* gpio)
{
    gpio->gpio_ctl_reg->GPIO_IC |= (1 << gpio->gpio_pin);           //清除中断标志
    gpio->gpio_ctl_reg->GPIO_IE &= (~(1 << gpio->gpio_pin));        //禁止中断
    free_irq(gpio->irq_num, gpio);                                  //解注册中断处理函数

    gpio->curr_use = GPIO_USE_UNKNOWN;                              //更新GPIO状态为未初始化
}

EXPORT_SYMBOL_GPL(gpio_intr_unregister);

/**
 * 设置GPIO输出指定的值
 * value: 0  输出低电平
 *        1  输出高电平
 * */
void set_gpio_value(hi3559_gpio_t* gpio, int value)
{
    if (gpio->curr_use == GPIO_USE_INTERRUPT) {
        gpio_intr_unregister(gpio);                                 //解注册中断
        gpio->gpio_ctl_reg->GPIO_DIR |= (1 << gpio->gpio_pin);      //将GPIO配置为通用输出
        gpio->curr_use = GPIO_USE_OUTPUT;                           //更新GPIO当前的用途状态
    } else if(gpio->curr_use == GPIO_USE_UNKNOWN || gpio->curr_use == GPIO_USE_INPUT) {
        //配置该管脚的配置寄存器, 将管脚复用成通用GPIO
        set_force_pull_up_disable(gpio);
        set_input_enable(gpio);
        set_smit_input_disable(gpio);
        set_gpio_quick(gpio);
        set_pull_down_disable(gpio);
        set_pull_up_disable(gpio);
        set_drive_level(gpio, 0);
        set_pin_as_gpio(gpio);

        gpio->gpio_ctl_reg->GPIO_DIR |= (1 << gpio->gpio_pin);      //将GPIO配置为通用输出
        gpio->curr_use = GPIO_USE_OUTPUT;                           //更新GPIO当前的用途状态
    }

    if (value == 0) {
        (*gpio->data_register) &= (~(1 << gpio->gpio_pin));
    } else if(value == 1) {
        (*gpio->data_register) |= (1 << gpio->gpio_pin);
    }
}

EXPORT_SYMBOL_GPL(set_gpio_value);

/**
 * 读取GPIO当前的电平状态
 * 返回值: 0  当前GPIO上为低电平
 *        1  当前GPIO上为高电平
 * */
int get_gpio_value(hi3559_gpio_t* gpio)
{
    //此处需要将GPIO配置为通用输入
    if (gpio->curr_use == GPIO_USE_UNKNOWN || gpio->curr_use == GPIO_USE_OUTPUT) {
        //配置该管脚的配置寄存器, 将管脚复用成通用GPIO
        set_force_pull_up_disable(gpio);
        set_input_enable(gpio);
        set_smit_input_disable(gpio);
        set_gpio_quick(gpio);
        set_pull_down_disable(gpio);
        set_pull_up_disable(gpio);
        set_drive_level(gpio, 0);
        set_pin_as_gpio(gpio);

        //配置引脚输出方向为输入
        gpio->gpio_ctl_reg->GPIO_DIR &= (~(1 << gpio->gpio_pin));
        //更新引脚用途标志
        gpio->curr_use = GPIO_USE_INPUT;
    }

    //读取引脚当前电平
    return (*gpio->data_register)&(1 << gpio->gpio_pin)?1:0;
}

EXPORT_SYMBOL_GPL(get_gpio_value);

static int sensor_hub_gpio_init(void)
{
    sys_debug_log("do sensor_hub_gpio_init \n");

    return 0;
}

static void sensor_hub_gpio_exit(void)
{
    sys_debug_log("do sensor_hub_gpio_exit \n");
}

module_init(sensor_hub_gpio_init);
module_exit(sensor_hub_gpio_exit);

MODULE_AUTHOR("WUJING");
MODULE_DESCRIPTION("HI3559 SENSOR HUB GPIO DRIVER");
MODULE_LICENSE("GPL");
