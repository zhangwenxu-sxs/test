/**
 * 这个驱动实现如下功能:
 *  1、提供接口供驱动模块向应用层上报消息
 *  2、提供接口供内核模块注册处理应用层消息的回调函数
 * */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/netlink.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <net/netlink.h>
#include <net/net_namespace.h>
#include <net/sock.h>
#include <asm/io.h>
#include "sn_netlink.h"

#define sn_nl_log(fmt, args...) printk("[SN_NETLINK][FUNC:%s][LINE:%d] "fmt, __FUNCTION__, __LINE__, ##args)

#define NETLINK_CUSTOM_STAR_NET 25      //自定义netlink号

static struct sock* nl_socket = NULL;   //用于向上层应用上报事件的netlink socket

/**
 * 挨个调用其他模块注册上来的回调函数
 * */
static void nl_msg_cb(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = (struct nlmsghdr*)skb->data;
    sn_nl_log("recive netlink message:%s \n", (char*)nlmsg_data(nlh));
}

int post_nl_msg(char* msg, unsigned int group)
{
    size_t message_size;
    struct nlmsghdr* nlh;
    struct sk_buff* skb;

    message_size = strlen(msg) + 1;

    skb = nlmsg_new(NLMSG_ALIGN(message_size), GFP_KERNEL);
    if (!skb) {
        sn_nl_log("Failed to allocate a new skb \n");
        return -1;
    }

    nlh = nlmsg_put(skb, 0, 1, NLMSG_DONE, message_size, 0);
    strncpy(nlmsg_data(nlh), msg, message_size);

    return nlmsg_multicast(nl_socket, skb, 0, group, GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(post_nl_msg);

static int sn_netlink_init(void)
{
    struct netlink_kernel_cfg nl_cfg = {
        .input = nl_msg_cb                  //指定netlink消息回调函数
    };

    nl_socket = netlink_kernel_create(&init_net, NETLINK_CUSTOM_STAR_NET, &nl_cfg);
    if (nl_socket == NULL) {
        sn_nl_log("create netlink socket failed \n");
        return -1;
    }

    return 0;
}

static void sn_netlink_exit(void)
{
    netlink_kernel_release(nl_socket);
}

module_init(sn_netlink_init);
module_exit(sn_netlink_exit);

MODULE_AUTHOR("WUJING");
MODULE_DESCRIPTION("STAR-NET netlink driver");
MODULE_LICENSE("GPL");
