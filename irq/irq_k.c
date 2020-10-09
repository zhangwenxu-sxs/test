#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
//#include <asm/arch/regs-gpio.h>
//#include <asm/hardware.h>

#define key_DEVICE_COUNT	1	
#define key_DEVICE_NAME		"key"

/*key字符设备结构体*/
struct key_dev			
{
	struct cdev 	cdev;			
	unsigned char 	*sth_of_key;	//预留
};
struct key_dev 	*key_device;

/*自动创建设备节点类*/
struct class *key_dev_class = NULL;
struct class_device *key_dev_class_dev = NULL;

/* 用来指定按键所用的外部中断引脚及中断触发方式, 名字 */
struct key_irq_desc {
    int 		irq;
    unsigned long 	flags;
    char 		*name;
};

static struct key_irq_desc key_irqs [] = {
    {IRQ_EINT19, IRQF_TRIGGER_FALLING, "KEY1"}, /* K1 */
    {IRQ_EINT11, IRQF_TRIGGER_FALLING, "KEY2"}, /* K2 */
    {IRQ_EINT2,  IRQF_TRIGGER_FALLING, "KEY3"}, /* K3 */
    {IRQ_EINT0,  IRQF_TRIGGER_FALLING, "KEY4"}, /* K4 */
};

/* 按键被按下的次数(准确地说，是发生中断的次数) */
static volatile int press_cnt [] = {0, 0, 0, 0};

/* 
等待队列: 
    当没有按键被按下时，如果有进程调用key_dev_read函数，它将休眠
*/
static DECLARE_WAIT_QUEUE_HEAD(key_waitq);

/* 中断事件标志, 中断服务程序将它置1，key_dev_read将它清0 */
static volatile int ev_press = 0;
static irqreturn_t keys_interrupt(int irq, void *dev_id)
{
    volatile int *press_cnt = (volatile int *)dev_id;
    
    *press_cnt = *press_cnt + 1; 		/* 按键计数加1 */
    ev_press = 1;                		/* 表示中断发生了 */
    wake_up_interruptible(&key_waitq); 	/* 唤醒休眠的进程 */
    
    return IRQ_RETVAL(IRQ_HANDLED);
}

/* 
应用程序对设备文件/dev/keys执行open(...)时，就会调用key_dev_open函数
*/
static int key_dev_open(struct inode *inode, struct file *file)
{
    int i;
    int err;
    
    for (i = 0; i < sizeof(key_irqs)/sizeof(key_irqs[0]); i++) {
        // 注册中断处理函数
        err = request_irq(key_irqs[i].irq, keys_interrupt, key_irqs[i].flags, key_irqs[i].name, (void *)&press_cnt[i]);
        if (err)
            break;
    }

    if (err) {
        // 释放已经注册的中断
        i--;
        for (; i >= 0; i--)
            free_irq(key_irqs[i].irq, (void *)&press_cnt[i]);
        return -EBUSY;
    }
    return 0;
}

/* 
应用程序对设备文件/dev/keys执行close(...)时，就会调用key_dev_close函数
*/
static int key_dev_close(struct inode *inode, struct file *file)
{
    int i;
    
    for (i = 0; i < sizeof(key_irqs)/sizeof(key_irqs[0]); i++) {
        // 释放已经注册的中断
        free_irq(key_irqs[i].irq, (void *)&press_cnt[i]);
    }
    return 0;
}

/* 
应用程序对设备文件/dev/keys执行read(...)时，就会调用key_dev_read函数
*/
static int key_dev_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    unsigned long err;
    
    /* 如果ev_press等于0，休眠 */
    wait_event_interruptible(key_waitq, ev_press);

    /* 执行到这里时，ev_press等于1，将它清0 */
    ev_press = 0;

    /* 将按键状态复制给用户 */
    err = copy_to_user(buff, (const void *)press_cnt, min(sizeof(press_cnt), count));
    //memset((void *)press_cnt, 0, sizeof(press_cnt));

    return err ? -EFAULT : 0;
}

/* 
这个结构是字符设备驱动程序的核心,当应用程序操作设备文件时所调用的open、read、write等函数，最终会调用这个结构中的对应函数
*/
static struct file_operations key_dev_fops = {
    .owner   =   THIS_MODULE,    /* 这是一个宏，指向编译模块时自动创建的__this_module变量 */
    .open    =   key_dev_open,
    .release =   key_dev_close, 
    .read    =   key_dev_read,
};

/*
执行“insmod key_int_driver.ko”命令时就会调用这个函数
*/
dev_t key_devno = 0;
int key_major = 0;
int key_minor = 0;
static int __init initialization_key_dev(void)
{
    /* 注册字符设备驱动程序
     * 参数为主设备号、设备名字、file_operations结构；
     * 这样，主设备号就和具体的file_operations结构联系起来了，
     * 操作主设备为key_major的设备文件时，就会调用key_dev_fops中的相关成员函数
     * KEY_MAJOR可以设为0，表示由内核自动分配主设备号
     */
	int ret = 0;
	 
	/*申请设备号*/
	printk("Before register Major = %d\n", key_major);
	if (key_major){
		key_devno = MKDEV(key_major, key_minor);
		ret = register_chrdev_region(key_devno, key_DEVICE_COUNT, key_DEVICE_NAME);
	} else {
		ret = alloc_chrdev_region(&key_devno, key_minor, key_DEVICE_COUNT, key_DEVICE_NAME);
		key_major = MAJOR(key_devno);
		key_minor = MINOR(key_devno);
	}	
	if (ret < 0)
	{
		printk("Can‘t get key Major : %d.\n", key_major);
		return ret;
	}
    printk("After register Major = %d\n", key_major);
	
	/* 申请设备结构体内存 */
	key_device = kmalloc(sizeof(struct key_dev), GFP_KERNEL);
	if (NULL == key_device) {
		printk("kmalloc failed.\n");
		unregister_chrdev_region(key_devno, key_DEVICE_COUNT);
		return -ENOMEM;
	}
	
	/* 字符设备注册 */
	cdev_init(&key_device->cdev, &key_dev_fops);
	key_device->cdev.owner = THIS_MODULE;
	key_device->cdev.ops = &key_dev_fops;
	ret = cdev_add(&key_device->cdev, key_devno, key_DEVICE_COUNT);
	if (ret < 0)
	{
		printk("Cdev add failed.\n");
		kfree(key_device);
		unregister_chrdev_region(key_devno, key_DEVICE_COUNT);
		return ret;
	}
	
	/* 自动生成设备节点 */
	key_dev_class = class_create(THIS_MODULE, "key_drv");	//类名：key_drv
	key_dev_class_dev = class_device_create(key_dev_class, key_devno, NULL, key_DEVICE_NAME);
	
	/* 模块初始化成功必须返回0 */
    printk(key_DEVICE_NAME " initialized OK.\n");
    return 0;
}

/*
执行”rmmod key_int_driver.ko”命令时就会调用这个函数 
 */
static void __exit cleanup_key_dev(void)
{
    /* 卸载驱动程序 */
	cdev_del(&key_device->cdev);
	kfree(key_device);
    	unregister_chrdev_region(key_devno, key_DEVICE_COUNT);
	class_device_unregister(key_dev_class_dev);
	class_destroy(key_dev_class);

	printk("Module unregister OK.\n");
}

/* 
这两行指定驱动程序的初始化函数和卸载函数 
*/
module_init(initialization_key_dev);
module_exit(cleanup_key_dev);

/*
insmod传参:insmod key_int_driver.ko key_major=key key_minor=0
*/
module_param(key_major, int, S_IRUGO);
module_param(key_minor, int, S_IRUGO);

/* 
描述驱动程序的一些信息，不是必须的 
*/
MODULE_AUTHOR("lhbo");             	// 驱动程序的作者
MODULE_DESCRIPTION("Key INT Driver");   // 一些描述信息
MODULE_LICENSE("GPL");                  // 遵循的协议
