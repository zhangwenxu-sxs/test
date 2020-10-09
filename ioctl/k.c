//头文件
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm-generic/ioctl.h>

#define LED_NUM_ON        _IOW('L',0x1122,int)
#define LED_NUM_OFF        _IOW('L',0x3344,int)
#define LED_ALL_ON        _IO('L',0x1234)
#define LED_ALL_OFF        _IO('L',0x5678)




//面向对象编程----设计设备的类型
struct s5pv210_led{
    unsigned int major;
    struct class * cls;
    struct device * dev;
    int data;
};
struct s5pv210_led *led_dev;



//实现设备操作接口
int led_open(struct inode *inode, struct file *filp)
{
    
    printk("--------^_^ %s------------\n",__FUNCTION__);


    return 0;
}
ssize_t led_write(struct file *filp, const char __user *buf, size_t size, loff_t *flags)
{
    int ret;
    printk("--------^_^ %s------------\n",__FUNCTION__);

    //应用空间数据转换为内核空间数据
    ret = raw_copy_from_user(&led_dev->data,buf,size);
    if(ret != 0){
        printk("copy_from_user error!\n");
        return -EFAULT;
    }

    if(led_dev->data){
        //点灯
        printk("light up\n");
    }else{
        //灭灯
        printk("light down\n");        
    }

    return size;
}

long led_ioctl(struct file *filp, unsigned int cmd , unsigned long args)
{
    int num = args+2;
    printk("--------^_^ %s------------\n",__FUNCTION__);
    switch(cmd){
        case LED_NUM_ON:        //将某个灯点亮
            if(num != 3 && num != 4)
                return -EINVAL;
            else
                printk("one light up\n");
            break;
        case LED_NUM_OFF:        //将某个灯点灭掉
            if(num != 3 && num != 4)
                return -EINVAL;
            else
                printk("one light down\n");
            break;
        case LED_ALL_ON:        //两个灯同时亮
            printk("two lights up in same time\n");    
            break;
        case LED_ALL_OFF:        //两个灯同时灭
            printk("two lights down in same time\n");   
            break;
        default:
            printk("unknow cmd!\n");
    }

    return 0;
}

int led_close(struct inode *inode, struct file *filp)
{
    printk("--------^_^ %s------------\n",__FUNCTION__);
    //灭灯
    printk("down light\n");
    return 0;
}


static struct file_operations fops = {
    .open = led_open,
    .write = led_write,
    .unlocked_ioctl = led_ioctl,
    .release = led_close,
};


//加载函数和卸载函数
static int __init led_init(void)   //加载函数-----在驱动被加载时执行
{
    int ret;
    printk("--------^_^ %s------------\n",__FUNCTION__);
    //0,实例化设备对象
    //参数1 ---- 要申请的空间的大小
    //参数2 ---- 申请的空间的标识
    led_dev = kzalloc(sizeof(struct s5pv210_led),GFP_KERNEL);
    if(IS_ERR(led_dev)){
        printk("kzalloc error!\n");
        ret = PTR_ERR(led_dev);
        return -ENOMEM;
    }
    
    //1,申请设备号
#if 0
    //静态申请主设备号
    led_dev->major = 256;
    ret = register_chrdev(led_dev->major,"led_drv",&fops);
    if(ret < 0){
        printk("register_chrdev error!\n");
        return -EINVAL;
    }
#else
    //动态申请主设备号
    led_dev->major = register_chrdev(0,"led_drv",&fops);
    if(led_dev->major < 0){
        printk("register_chrdev error!\n");
        ret =  -EINVAL;
        goto err_kfree;
    }
#endif

    //2,创建设备文件-----/dev/led1
    led_dev->cls = class_create(THIS_MODULE,"led_cls");
    if(IS_ERR(led_dev->cls)){
        printk("class_create error!\n");
        ret = PTR_ERR(led_dev->cls);
        goto err_unregister;
    }
    
    led_dev->dev = device_create(led_dev->cls,NULL,MKDEV(led_dev->major,0),NULL,"led");
    if(IS_ERR(led_dev->dev)){
        printk("device_create error!\n");
        ret = PTR_ERR(led_dev->dev);
        goto err_class;
    }


    //3,硬件初始化----地址映射

    


    
    return 0;

err_class:
    class_destroy(led_dev->cls);

err_unregister:
    unregister_chrdev(led_dev->major,"led_drv");
    
err_kfree:
    kfree(led_dev);
    return ret;

    
}

static void __exit led_exit(void)   //卸载函数-----在驱动被卸载时执行
{
    printk("--------^_^ %s------------\n",__FUNCTION__);
    device_destroy(led_dev->cls,MKDEV(led_dev->major,0));
    class_destroy(led_dev->cls);
    unregister_chrdev(led_dev->major,"led_drv");
    kfree(led_dev);
}

//声明和认证
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
