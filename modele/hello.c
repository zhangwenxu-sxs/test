#include <linux/module.h>
#include <linux/kernel.h>





static int test_init(void)
{
    printk("test ko insmod successful!\n");
    return 0;
}
 
static void test_exit(void)
{
    printk("test ko rmmod successful!\n");
}
 
module_init(test_init);
module_exit(test_exit);
 
MODULE_AUTHOR("ZHANGWENXU");
MODULE_DESCRIPTION("Driver TEST");
MODULE_LICENSE("GPL");
