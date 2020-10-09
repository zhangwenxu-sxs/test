#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/slab.h>				// kzalloc和kfree的头文件

typedef struct 
{
	dev_t dev_no;
	char devname[20];
	char classname[20];
	struct cdev demo_cdev;
	struct class *cls;
	struct device *dev;
	struct mutex my_mutex;			// 定义一个互斥体，在init函数中进行初始化
}demo_struct;
demo_struct *my_demo_dev = NULL;	// 定义一个设备结构体指针，指向NULL

static int demo_open(struct inode *inode, struct file *filp)
{
	/* 首先判断设备是否可用 */
	if( mutex_lock_interruptible(&my_demo_dev->my_mutex) )		// 访问共享资源之前获取互斥体，成功获取返回0
	{
		return -ERESTARTSYS;						// 不能获取返回错误码
	}
	
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);
	
	return 0;

}

static int demo_release(struct inode *inode, struct file *filp)
{
	/* 释放互斥量 */
	mutex_unlock(&my_demo_dev->my_mutex);

	printk("%s -- %d.\n", __FUNCTION__, __LINE__);
	
	return 0;
}

struct file_operations demo_ops = {
	.open = demo_open,
	.release=  demo_release,
};

static int __init demo_init(void)
{
	int ret;

	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	/* 开辟空间 */
	my_demo_dev =  kzalloc(sizeof(demo_struct), GFP_KERNEL);
	if ( IS_ERR(my_demo_dev) )
	{
		printk("kzalloc failed.\n");
		ret = PTR_ERR(my_demo_dev);
		goto kzalloc_err;
	}
	strcpy(my_demo_dev->devname, "demo_chrdev");	// 给设备名字赋值
	strcpy(my_demo_dev->classname, "demo_class");	// 给设备类的名字赋值

	mutex_init(&my_demo_dev->my_mutex);				// 初始化互斥体
	
	ret = alloc_chrdev_region(&my_demo_dev->dev_no, 0, 0, my_demo_dev->devname);
	if (ret)
	{
		printk("alloc_chrdev_region failed.\n");
		goto region_err;
	}

	cdev_init(&my_demo_dev->demo_cdev, &demo_ops);

	ret = cdev_add(&my_demo_dev->demo_cdev, my_demo_dev->dev_no, 1);
	if (ret < 0)
	{
		printk("cdev_add failed.\n");
		goto add_err;
	}

	my_demo_dev->cls = class_create(THIS_MODULE, my_demo_dev->classname);		/* 在目录/sys/class/.. */
	if ( IS_ERR(my_demo_dev->cls) )
	{
		ret = PTR_ERR(my_demo_dev->cls);
		printk("class_create failed.\n");
		goto cls_err;
	}

	my_demo_dev->dev = device_create(my_demo_dev->cls, NULL, my_demo_dev->dev_no, NULL, "chrdev%d", 0);	/* 在目录/dev/.. */
	if ( IS_ERR(my_demo_dev->dev) )
	{
		ret = PTR_ERR(my_demo_dev->dev);
		printk("device_create failed.\n");
		goto dev_err;
	}
	
	return 0;

dev_err:
	class_destroy(my_demo_dev->cls);	
cls_err:
	cdev_del(&my_demo_dev->demo_cdev);
add_err:
	unregister_chrdev_region(my_demo_dev->dev_no, 1);
region_err:
	kfree(my_demo_dev);		// 释放空间，避免内存泄漏
kzalloc_err:
	return ret;
}


static void __exit demo_exit(void)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	device_destroy(my_demo_dev->cls, my_demo_dev->dev_no);
	class_destroy(my_demo_dev->cls);
	cdev_del(&my_demo_dev->demo_cdev);
	unregister_chrdev_region(my_demo_dev->dev_no, 1);
	kfree(my_demo_dev);		// 释放空间，避免内存泄漏
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");

