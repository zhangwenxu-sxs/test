#include <linux/init.h>         /* printk() */
#include <linux/module.h>       /* __init __exit */
static int  __init  hello_init(void)      /*模块加载函数，通过insmod命令加载模块时，被自动执行*/
{ 
  printk(KERN_INFO " Hello World enter\n"); 
return 0; 
} 
static void  __exit  hello_exit(void)    /*模块卸载函数，当通过rmmod命令卸载时，会被自动执行*/
{ 
  printk(KERN_INFO " Hello World exit\n "); 
} 
module_init(hello_init); 
module_exit(hello_exit); 
MODULE_AUTHOR("dengwei");           /*模块作者，可选*/
MODULE_LICENSE("Dual BSD/GPL");     /*模块许可证明，描述内核模块的许可权限，必须*/
MODULE_DESCRIPTION("A simple Hello World Module"); /*模块说明，可选*/
MODULE_ALIAS("a simplest module");                  /*模块说明，可选*/
