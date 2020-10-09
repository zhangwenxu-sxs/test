/*
 * Linux rfkill helper functions for driver wrappers
 * Copyright (c) 2010, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"
#include <fcntl.h>

#include "utils/common.h"
#include "utils/eloop.h"
#include "rfkill.h"

#define RFKILL_EVENT_SIZE_V1 8

struct rfkill_event {
	u32 idx;
	u8 type;        //代表设备类型
	u8 op;          //op变量代表 rfkill 事件的类型，目前可取值由 RFKILL_OP_ADD、 RFKILL_OP_DEL 等
	u8 soft;
	u8 hard;
} STRUCT_PACKED;

enum rfkill_operation {
	RFKILL_OP_ADD = 0,
	RFKILL_OP_DEL,
	RFKILL_OP_CHANGE,
	RFKILL_OP_CHANGE_ALL,
};

enum rfkill_type {
	RFKILL_TYPE_ALL = 0,
	RFKILL_TYPE_WLAN,
	RFKILL_TYPE_BLUETOOTH,
	RFKILL_TYPE_UWB,
	RFKILL_TYPE_WIMAX,
	RFKILL_TYPE_WWAN,
	RFKILL_TYPE_GPS,
	RFKILL_TYPE_FM,
	NUM_RFKILL_TYPES,
};


struct rfkill_data {
	struct rfkill_config *cfg;
	int fd;
	int blocked;
};


static void rfkill_receive(int sock, void *eloop_ctx, void *sock_ctx)
{
	struct rfkill_data *rfkill = eloop_ctx;
	struct rfkill_event event;
	ssize_t len;
	int new_blocked;

	len = read(rfkill->fd, &event, sizeof(event));
	if (len < 0) {
		wpa_printf(MSG_ERROR, "rfkill: Event read failed: %s",
			   strerror(errno));
		return;
	}
	if (len != RFKILL_EVENT_SIZE_V1) {
		wpa_printf(MSG_DEBUG, "rfkill: Unexpected event size "
			   "%d (expected %d)",
			   (int) len, RFKILL_EVENT_SIZE_V1);
		return;
	}
	wpa_printf(MSG_DEBUG, "rfkill: event: idx=%u type=%d "
		   "op=%u soft=%u hard=%u",
		   event.idx, event.type, event.op, event.soft,
		   event.hard);
	if (event.op != RFKILL_OP_CHANGE || event.type != RFKILL_TYPE_WLAN)
		return;

	if (event.hard) {
		wpa_printf(MSG_INFO, "rfkill: WLAN hard blocked");
		new_blocked = 1;
	} else if (event.soft) {
		wpa_printf(MSG_INFO, "rfkill: WLAN soft blocked");
		new_blocked = 1;
	} else {
		wpa_printf(MSG_INFO, "rfkill: WLAN unblocked");
		new_blocked = 0;
	}

	if (new_blocked != rfkill->blocked) {
		rfkill->blocked = new_blocked;
		if (new_blocked)
			rfkill->cfg->blocked_cb(rfkill->cfg->ctx);
		else
			rfkill->cfg->unblocked_cb(rfkill->cfg->ctx);
	}
}


struct rfkill_data * rfkill_init(struct rfkill_config *cfg)
{
	struct rfkill_data *rfkill;
	struct rfkill_event event;
	ssize_t len;

	rfkill = os_zalloc(sizeof(*rfkill));
	if (rfkill == NULL)
		return NULL;

	rfkill->cfg = cfg;
    //O_RDONLY 标志表示 driver_nl80211 只读取 rfkill 事件，而不会去操作 rfkill 模块
	rfkill->fd = open("/dev/rfkill", O_RDONLY);
	if (rfkill->fd < 0) {
		wpa_printf(MSG_INFO, "rfkill: Cannot open RFKILL control "
			   "device");
		goto fail;
	}

    //设置 I/O 操作为非阻塞式
	if (fcntl(rfkill->fd, F_SETFL, O_NONBLOCK) < 0) {
		wpa_printf(MSG_ERROR, "rfkill: Cannot set non-blocking mode: "
			   "%s", strerror(errno));
		goto fail2;
	}

	for (;;) {
        //读取 /dev/rfkill 中已有的事件信息。 rfkill 事件信息保存在 rfkill_event 结构体中
		len = read(rfkill->fd, &event, sizeof(event));
		if (len < 0) {
			if (errno == EAGAIN)
				break; /* No more entries 无数据可读，则跳出循环*/
			wpa_printf(MSG_ERROR, "rfkill: Event read failed: %s",
				   strerror(errno));
			break;      //其他错误也跳出循环
		}
		if (len != RFKILL_EVENT_SIZE_V1) {
			wpa_printf(MSG_DEBUG, "rfkill: Unexpected event size "
				   "%d (expected %d)",
				   (int) len, RFKILL_EVENT_SIZE_V1);
			continue;
		}
		wpa_printf(MSG_DEBUG, "rfkill: initial event: idx=%u type=%d "
			   "op=%u soft=%u hard=%u",
			   event.idx, event.type, event.op, event.soft,
			   event.hard);
        //op变量代表 rfkill 事件的类型，目前可取值由 RFKILL_OP_ADD、 RFKILL_OP_DEL 等
        //type 代表该 rfkill 事件所对应设备的类型：如 RFKILL_TYPE_WLAN/ RFKILL_TYPE_BLUETOOTH等。
		if (event.op != RFKILL_OP_ADD ||
		    event.type != RFKILL_TYPE_WLAN)
			continue;
		if (event.hard) {//是否为hard block， 指不能通过软件来重启的 RF 设备
			wpa_printf(MSG_INFO, "rfkill: WLAN hard blocked");
			rfkill->blocked = 1;
		} else if (event.soft) {//用软件来重启 RF 设备
			wpa_printf(MSG_INFO, "rfkill: WLAN soft blocked");
			rfkill->blocked = 1;
		}
	}

    //为 eloop 注册一个读事件，一旦 rfkill 有新的事件到来， 则 eloop 会触发 rfkill_receive 函数被调用
	eloop_register_read_sock(rfkill->fd, rfkill_receive, rfkill, NULL);

	return rfkill;

fail2:
	close(rfkill->fd);
fail:
	os_free(rfkill);
	return NULL;
}


void rfkill_deinit(struct rfkill_data *rfkill)
{
	if (rfkill == NULL)
		return;

	if (rfkill->fd >= 0) {
		eloop_unregister_read_sock(rfkill->fd);
		close(rfkill->fd);
	}

	os_free(rfkill->cfg);
	os_free(rfkill);
}


int rfkill_is_blocked(struct rfkill_data *rfkill)
{
	if (rfkill == NULL)
		return 0;

	return rfkill->blocked;
}
