#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kthread.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define MOTOR_PROC_MSG_BUFF_SIZE (8 * 1024)     //提供给驱动写日志信息到proc文件系统的buff大小


//函数声明
static int motor_fops_open(struct inode* inode, struct file* file);
static int motor_fops_release(struct inode* inode, struct file* file);
static ssize_t motor_fops_read(struct file* file, char __user* buffer, size_t length, loff_t* offset);
static ssize_t motor_fops_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset);



static struct file_operations motor_fops = {
    .open = motor_fops_open,
    .release = motor_fops_release,
    .read = motor_fops_read,
    .write = motor_fops_write
};



static char motor_msg_buf[MOTOR_PROC_MSG_BUFF_SIZE];        //用于存放转换成字符串后的fpga_statistic信息
static unsigned int index = 0;                              //用于遍历proc_msg_buf

static int int_tmp;

static int motor_show_statistic(void)
{
    //将motor统计信息转换成字符串写入proc_msg_buf
    sprintf(motor_msg_buf, "\n=========================== MESSAGE ===========================\n");
    index = strlen(motor_msg_buf);
    sprintf(motor_msg_buf + index, "RECEIVE:%d \n", int_tmp);
    index = strlen(motor_msg_buf);
    return 0;
}

static int motor_fops_open(struct inode* inode, struct file* file)
{
    memset(motor_msg_buf, 0, sizeof(motor_msg_buf));

    index = 0;  //index恢复到0, 准备开始写fpga统计信息
    motor_show_statistic();
    index = 0;  //index恢复到0, 准备开始读fpga统计信息

    return 0;
}

static ssize_t motor_fops_read(struct file* file, char __user* buffer, size_t length, loff_t* offset)
{
    int ret;
    int copy_count = 0;     //要拷贝给应用的字节数
    size_t reserve_char = strlen(motor_msg_buf) - index + 1; //proc_msg_buf中未拷贝的字节数

    copy_count = min(length, reserve_char);
    //ret = copy_to_user(buffer, motor_msg_buf + index, copy_count);
    ret = raw_copy_to_user(buffer, motor_msg_buf + index, copy_count);
    index += (copy_count - ret);

    return copy_count - ret;
}



static ssize_t motor_fops_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset)
{
   
    return length;
}

static int motor_fops_release(struct inode* inode, struct file* file)
{
    return 0;
}



static int motor_init(void)
{
	//创建/proc/motor_monitor文件
    proc_create("test_proc", S_IRWXU | S_IRWXG | S_IRWXO, NULL, &motor_fops);

    return 0;
}

static void motor_exit(void)
{
	remove_proc_entry("test_proc", NULL);
}

module_init(motor_init);
module_exit(motor_exit);

MODULE_AUTHOR("ZHANGWENXU");
MODULE_DESCRIPTION("Device Detect Driver");
MODULE_LICENSE("GPL");
