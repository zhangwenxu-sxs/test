/**
 * 由于有些芯片是免驱的, 但是需要某些控制信号, 例如复位、
 * 电源控制之类的信号。 单独实现该驱动会很不灵活、兼容性
 * 差, 所以将这类信号集中到一个驱动中, 由设备树直接配置,
 * 同时向应用提供proc文件系统接口, 用于实现应用对这些信
 * 号的控制.
 * */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/netlink.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/gpio.h>
#include <linux/proc_fs.h>
#include <linux/switch.h>
#include <linux/platform_device.h>

#include <asm/io.h>

#include "gpio_ctrl_hub.h"
#include "sensor_hub_gpio.h"

static struct proc_dir_entry* proc_dentry = NULL;               //存放文件节点的目录项
static struct list_head chip_list = {&chip_list, &chip_list};   //存放芯片的链表

static void chip_init(chip_t* chip)
{
    mutex_init(&chip->chip_mutex);

    INIT_LIST_HEAD(&chip->chip_node);
    INIT_LIST_HEAD(&chip->operations);
}

static void chip_prepare_proc_msg(chip_t* chip)
{
    int i = 0;
    int idx = 0;
    action_t* action;
    operation_t* operation;

    sprintf(chip->proc_buffer + idx, "\n");
    idx = strlen(chip->proc_buffer);

    //打印芯片名
    sprintf(chip->proc_buffer + idx, "chip_name:%s \n", chip->name);
    idx = strlen(chip->proc_buffer);

    //打印需要控制的GPIO
    sprintf(chip->proc_buffer + idx, "GPIOS[ ");
    idx = strlen(chip->proc_buffer);
    while (1) {
        if (chip->gpios[i] == -1) {
            break;
        } else {
            if (chip->gpios[i] < SHUB_GPIO_OFFSET) {
                sprintf(chip->proc_buffer + idx, "GPIO%d_%d, ", chip->gpios[i] / 8, chip->gpios[i] % 8);
            } else if(chip->gpios[i] >= SHUB_GPIO_OFFSET) {
                sprintf(chip->proc_buffer + idx, "SHUB_GPIO%d_%d, ", (chip->gpios[i] - SHUB_GPIO_OFFSET) / 8, (chip->gpios[i] - SHUB_GPIO_OFFSET) % 8);
            }
            idx = strlen(chip->proc_buffer);
        }

        i += 1;
        if (i == CHIP_MAX_GPIO_CNT) {
            break;
        }
    }
    sprintf(chip->proc_buffer + idx, "] \n");
    idx = strlen(chip->proc_buffer);

    //打印支持的操作
    list_for_each_entry(operation, &chip->operations, operation_node) {
        sprintf(chip->proc_buffer + idx, "\noperation: %s \n", operation->name);
        idx = strlen(chip->proc_buffer);
        sprintf(chip->proc_buffer + idx, "    should_do: %s \n", operation->should_do ? "yes" : "no");
        idx = strlen(chip->proc_buffer);

        list_for_each_entry(action, &operation->actions, action_node) {
            if (action->val == 2) {
                if (action->gpio < SHUB_GPIO_OFFSET) {
                    sprintf(chip->proc_buffer + idx, "    set GPIO%d_%d as input \n", action->gpio / 8, action->gpio % 8);
                } else if(action->gpio >= SHUB_GPIO_OFFSET) {
                    sprintf(chip->proc_buffer + idx, "    set SHUB_GPIO%d_%d as input \n", (action->gpio - SHUB_GPIO_OFFSET) / 8,
                                                                                           (action->gpio - SHUB_GPIO_OFFSET) % 8);
                }
                idx = strlen(chip->proc_buffer);
            } else {
                if (action->gpio < SHUB_GPIO_OFFSET) {
                    sprintf(chip->proc_buffer + idx, "    set GPIO%d_%d %s for %dus \n", action->gpio / 8, action->gpio % 8,
                                                                        action->val == 1 ? "HIGH" : "LOW", action->delay_us);
                } else if(action->gpio >= SHUB_GPIO_OFFSET) {
                    sprintf(chip->proc_buffer + idx, "    set SHUB_GPIO%d_%d %s for %dus \n", (action->gpio - SHUB_GPIO_OFFSET) / 8,
                                                                                              (action->gpio - SHUB_GPIO_OFFSET) % 8,
                                                                                               action->val == 1 ? "HIGH" : "LOW", action->delay_us);
                }
                idx = strlen(chip->proc_buffer);
            }
        }
    }

    sprintf(chip->proc_buffer + idx, "\n");
}

static void print_action(action_t* action)
{
    if (action->val == 2) {
        if (action->gpio < SHUB_GPIO_OFFSET) {
            printk("    set GPIO%d_%d as input \n", action->gpio / 8, action->gpio % 8);
        } else if(action->gpio >= SHUB_GPIO_OFFSET) {
            printk("    set SHUB_GPIO%d_%d as input \n", (action->gpio - SHUB_GPIO_OFFSET) / 8,
                                                         (action->gpio - SHUB_GPIO_OFFSET) % 8);
        }
    } else {
        if (action->gpio < SHUB_GPIO_OFFSET) {
            printk("    set GPIO%d_%d %s for %dus \n", action->gpio / 8, action->gpio % 8, action->val == 1 ? "HIGH" : "LOW", action->delay_us);
        } else if(action->gpio >= SHUB_GPIO_OFFSET) {
            printk("    set SHUB_GPIO%d_%d %s for %dus \n", (action->gpio - SHUB_GPIO_OFFSET) / 8,
                                                            (action->gpio - SHUB_GPIO_OFFSET) % 8,
                                                             action->val == 1 ? "HIGH" : "LOW", action->delay_us);
        }
    }
}

static void print_actions(struct list_head* action_list)
{
    action_t* action;

    list_for_each_entry(action, action_list, action_node) {
        print_action(action);
    }
}

static void do_operation(chip_t* chip, char* operation_name)
{
    action_t* action;
    operation_t* operation;
    hi3559_gpio_t* shub_gpio;

    gpio_hub_dbg("chip:%s  operation:%s \n", chip->name, operation_name);
    //遍历operations链表, 查找用户要执行的operation, 若找到 则执行组成该operation的所有action
    list_for_each_entry(operation, &chip->operations, operation_node) {
        if (strstr(operation_name, operation->name)) {
            list_for_each_entry(action, &operation->actions, action_node) {
                print_action(action);

                if (action->gpio < SHUB_GPIO_OFFSET) {
                    gpio_request(action->gpio, NULL);
                    if (action->val == 2) {
                        gpio_direction_input(action->gpio);
                    } else {
                        gpio_direction_output(action->gpio, action->val);
                    }

                    if (action->delay_us > 0) {
                        usleep_range(action->delay_us, action->delay_us);
                    }

                    gpio_free(action->gpio);
                } else if (action->gpio >= SHUB_GPIO_OFFSET) {
                    shub_gpio = create_gpio((action->gpio - SHUB_GPIO_OFFSET) / 8, (action->gpio - SHUB_GPIO_OFFSET) % 8);
                    if (shub_gpio == NULL) {
                        continue;
                    }

                    if (action->val == 2) {
                        get_gpio_value(shub_gpio);
                    } else {
                        set_gpio_value(shub_gpio, action->val);
                    }

                    if (action->delay_us > 0) {
                        usleep_range(action->delay_us, action->delay_us);
                    }

                    destroy_gpio(shub_gpio);
                }
            }

            return;
        }
    }

    printk("operation[%s] not found, please do (cat /proc/gpio_ctrl_hub/%s) for more information \n", operation_name, chip->name);
}

static void chip_do_should_do(chip_t* chip)
{
    operation_t* operation;

    //找出那些被标记为should_do的operation并执行
    list_for_each_entry(operation, &chip->operations, operation_node) {
        if (operation->should_do) {
            do_operation(chip, operation->name);
        }
    }
}

static int gpio_hub_open(struct inode* inode, struct file* file)
{
    int ret;
    chip_t* chip;
    const char* file_name = file->f_path.dentry->d_name.name;

    list_for_each_entry(chip, &chip_list, chip_node) {
        if (strcmp(file_name, chip->name) == 0) {
            ret = mutex_trylock(&chip->chip_mutex);
            if (ret) {
                file->private_data = chip;
                chip->buffer_idx = 0;
                break;
            } else {
                gpio_hub_log("this file is operating by other process \n");
                return -1;
            }
        }
    }

    return 0;
}

static int gpio_hub_release(struct inode* inode, struct file* file)
{
    chip_t* chip = (chip_t*)file->private_data;

    mutex_unlock(&chip->chip_mutex);

    return 0;
}

static ssize_t gpio_hub_read(struct file* file, char __user * buffer, size_t len, loff_t* offset)
{
    int ret;
    int copy_cnt;
    chip_t* chip = (chip_t*)file->private_data;
    int reserve = strlen(chip->proc_buffer) - chip->buffer_idx + 1;

    copy_cnt = min(len, reserve);
    ret = copy_to_user(buffer, chip->proc_buffer + chip->buffer_idx, copy_cnt);
    chip->buffer_idx += (copy_cnt - ret);

    return copy_cnt - ret;
}

static ssize_t gpio_hub_write(struct file* file, const char __user * buffer, size_t len, loff_t* offset)
{
    chip_t* chip = (chip_t*)file->private_data;

    do_operation(chip, buffer);

    return len;
}

static struct file_operations gpio_hub_fops = {
    .open = gpio_hub_open,
    .release = gpio_hub_release,
    .read = gpio_hub_read,
    .write = gpio_hub_write
};

static int gpio_ctrl_hub_probe(struct platform_device* pdev)
{
    //解析设备树文件, 并初始化芯片链表
    int ret;
    int act_cnt;
    int operation_cnt;
    char action[64];
    chip_t* tmp_chip;
    action_t* tmp_action;
    operation_t* tmp_operation;

    struct device_node* child;              //子节点指针(用于遍历所有芯片)
    struct device_node* operation_node;     //用于遍历芯片所支持的操作
    struct device_node* operations_node;    //维护着芯片所支持的操作的设备树节点

    int gpio_indicator;
    int gpio_value;
    int delay_us;

    int idx;
    unsigned int val;
    unsigned int reg_addr;
    volatile unsigned int* reg_ptr;

    for_each_child_of_node(pdev->dev.of_node, child) {
        tmp_chip = (chip_t*)kzalloc(sizeof(chip_t), GFP_KERNEL);
        if (tmp_chip == NULL) {
            gpio_hub_log("alloc memory for chip failed \n");
            continue;
        }

        chip_init(tmp_chip);    //初始化相关变量

        //读取寄存器配置，配置相关寄存器
        idx = 0;
        while (1) {
            ret = of_property_read_u32_index(child, "reg_cfg", idx, (unsigned int*)&reg_addr);
            if (ret != 0) {
                break;
            }

            idx += 1;

            ret = of_property_read_u32_index(child, "reg_cfg", idx, &val);
            if (ret != 0) {
                break;
            }

            idx += 1;

            reg_ptr = (volatile unsigned int*)ioremap(reg_addr, 4);
            if (reg_ptr != NULL) {
                *reg_ptr = val;
                iounmap((void*)reg_ptr);
                gpio_hub_log("set reg[0x%x] to val[0x%x] \n", reg_addr, val);
            }
        }

        //初始化芯片名
        ret = of_property_read_string(child, "chip_name", (const char**)(&tmp_chip->name));
        if (ret != 0) {
            gpio_hub_log("get property[%s] failed \n", "chip_name");
            kfree(tmp_chip);
            continue;
        }

        //获取芯片所需要操作的GPIO管脚
        for (idx = 0; idx < CHIP_MAX_GPIO_CNT; idx++) {
            ret = of_property_read_u32_index(child, "gpios", idx, &tmp_chip->gpios[idx]);
            if (ret != 0) {
                break;
            }
        }

        for (;idx < CHIP_MAX_GPIO_CNT; idx++) {
            tmp_chip->gpios[idx] = -1;
        }

        //解析芯片所支持的所有operation
        operations_node = of_get_child_by_name(child, "operations");
        if (operations_node) {
            //将芯片支持的operation解析出来放到链表
            for_each_child_of_node(operations_node, operation_node) {
                tmp_operation = (operation_t*)kzalloc(sizeof(operation_t), GFP_KERNEL);
                if (tmp_operation == NULL) {
                    gpio_hub_log("alloc memory for operation failed \n");
                    continue;
                }

                INIT_LIST_HEAD(&tmp_operation->actions);

                ret = of_property_read_string(operation_node, "operation_name", (const char**)(&tmp_operation->name));
                if (ret != 0) {
                    gpio_hub_log("get operation_name of %s failed, please check your device tree \n", operation_node->full_name);
                    kfree(tmp_operation);
                    continue;
                }

                ret = of_property_read_u32(operation_node, "should_do", &tmp_operation->should_do);
                if (ret != 0) {
                    tmp_operation->should_do = 0;
                }

                //解析组成这个operation的所有action
                act_cnt = 1;
                while (1) {
                    sprintf(action, "%s%d", ACTION_ATTR_PREFIX, act_cnt);

                    ret = of_property_read_u32_index(operation_node, action, 0, &gpio_indicator);
                    ret += of_property_read_u32_index(operation_node, action, 1, &gpio_value);
                    ret += of_property_read_u32_index(operation_node, action, 2, &delay_us);

                    if (ret != 0) {
                        break;      //解析完所有的action了, 退出
                    }

                    tmp_action = (action_t*)kzalloc(sizeof(action_t), GFP_KERNEL);
                    if (tmp_action == NULL) {
                        break;
                    }

                    tmp_action->gpio = tmp_chip->gpios[gpio_indicator];
                    tmp_action->val = gpio_value;
                    tmp_action->delay_us = delay_us;

                    //将这个操作加进operation的action链表
                    list_add_tail(&tmp_action->action_node, &tmp_operation->actions);

                    act_cnt += 1;
                }
                //将这个operation加进芯片的operation链表
                list_add_tail(&tmp_operation->operation_node, &tmp_chip->operations);
            }
        } else {
            gpio_hub_log("%s do not has any operation \n", tmp_chip->name);
        }

        //打印芯片信息到proc_buffer
        chip_prepare_proc_msg(tmp_chip);

        //执行那些被标记为should_do的operation
        chip_do_should_do(tmp_chip);

        //创建proc文件节点
        ret = of_property_read_u32(child, "need_proc", &tmp_chip->need_proc);
        if (ret != 0) {
            tmp_chip->need_proc = 0;
        }

        if (tmp_chip->need_proc) {
            proc_create(tmp_chip->name, S_IRWXU | S_IRWXG | S_IRWXO, proc_dentry, &gpio_hub_fops);
        }

        //将tmp_chip加入芯片链表
        list_add(&tmp_chip->chip_node, &chip_list);
    }

    return 0;
}

static int gpio_ctrl_hub_remove(struct platform_device* pdev)
{
    chip_t* chip;
    chip_t* next_chip;
    action_t* action;
    action_t* next_action;
    operation_t* operation;
    operation_t* next_operation;

    list_for_each_entry_safe(chip, next_chip, &chip_list, chip_node) {
        list_for_each_entry_safe(operation, next_operation, &chip->operations, operation_node) {
            list_for_each_entry_safe(action, next_action, &operation->actions, action_node) {
                list_del(&action->action_node);
                kfree(action);
            }
            list_del(&operation->operation_node);
            kfree(operation);
        }
        list_del(&chip->chip_node);
        kfree(chip);
    }

    return 0;
}

static struct of_device_id gpio_ctrl_hub_ids[] = {
    {.compatible = "gpio_ctrl_hub"},
    {}
};

static struct platform_driver gpio_ctrl_hub_drv = {
    .driver = {
        .name = "gpio_ctrl_hub",
        .of_match_table = of_match_ptr(gpio_ctrl_hub_ids)
    },
    .probe = gpio_ctrl_hub_probe,
    .remove = gpio_ctrl_hub_remove
};

static int gpio_ctrl_hub_init(void)
{
    proc_dentry = proc_mkdir("gpio_ctrl_hub", NULL);
    if (proc_dentry == NULL) {
        gpio_hub_log("create /proc/starnet/gpio_ctrl_hub failed \n");
        return -1;
    }

    return platform_driver_register(&gpio_ctrl_hub_drv);
}

static void gpio_ctrl_hub_exit(void)
{
    proc_remove(proc_dentry);
    platform_driver_unregister(&gpio_ctrl_hub_drv);
}

module_init(gpio_ctrl_hub_init);
module_exit(gpio_ctrl_hub_exit);

MODULE_AUTHOR("WUJING");
MODULE_DESCRIPTION("GPIO_CTRL_HUB DRIVER");
MODULE_LICENSE("GPL");
