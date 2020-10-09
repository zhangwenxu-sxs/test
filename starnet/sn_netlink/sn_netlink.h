#ifndef __SN_NETLINK_H
#define __SN_NETLINK_H

#define STAR_NET_DRIVER_MSG_GRP 1   //用于上报我们自己的驱动消息的netlink消息组

//向NETLINK_CUSTOM_STAR_NET family的group组广播netlink消息
int post_nl_msg(char* msg, unsigned int group);

#endif
