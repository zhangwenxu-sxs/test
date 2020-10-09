#ifndef __CAT9883CDRV_H__
#define __CAT9883CDRV_H__

#define CAT9883_LOG(fmt, args...) printk("[CAT9883][FUNC:%s][LINE:%d]"fmt, __FUNCTION__, __LINE__, ##args);

#endif
