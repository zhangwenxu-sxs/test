/*
 * 80211.c
 *
 *  Created on: 2020年8月14日
 *      Author: zhangwenxu
 */


#include "driver_nl80211.c"



static int wpa_driver_nl80211_scan(struct i802_bss *bss,
                   struct wpa_driver_scan_params *params)
{
    driver_nl80211_log("----------");
    printf("--------------------wpa_driver_nl80211_scan----------------------\n");
    struct wpa_driver_nl80211_data *drv = bss->drv;
    int ret = -1, timeout;
    struct nl_msg *msg = NULL;

    wpa_dbg(drv->ctx, MSG_DEBUG, "nl80211: scan request");
    drv->scan_for_auth = 0;

    msg = nl80211_scan_common(drv, NL80211_CMD_TRIGGER_SCAN, params,
                  bss->wdev_id_set ? &bss->wdev_id : NULL);
    if (!msg)
        return -1;

    if (params->p2p_probe) {
        struct nlattr *rates;

        wpa_printf(MSG_DEBUG, "nl80211: P2P probe - mask SuppRates");

        rates = nla_nest_start(msg, NL80211_ATTR_SCAN_SUPP_RATES);
        if (rates == NULL)
            goto nla_put_failure;

        /*
         * Remove 2.4 GHz rates 1, 2, 5.5, 11 Mbps from supported rates
         * by masking out everything else apart from the OFDM rates 6,
         * 9, 12, 18, 24, 36, 48, 54 Mbps from non-MCS rates. All 5 GHz
         * rates are left enabled.
         */
        NLA_PUT(msg, NL80211_BAND_2GHZ, 8,
            "\x0c\x12\x18\x24\x30\x48\x60\x6c");
        nla_nest_end(msg, rates);

        NLA_PUT_FLAG(msg, NL80211_ATTR_TX_NO_CCK_RATE);
    }

    ret = send_and_recv_msgs(drv, msg, NULL, NULL);
    msg = NULL;
    if (ret) {
        wpa_printf(MSG_DEBUG, "nl80211: Scan trigger failed: ret=%d "
               "(%s)", ret, strerror(-ret));
#ifdef HOSTAPD
        if (is_ap_interface(drv->nlmode)) {
            /*
             * mac80211 does not allow scan requests in AP mode, so
             * try to do this in station mode.
             */
            if (wpa_driver_nl80211_set_mode(
                    bss, NL80211_IFTYPE_STATION))
                goto nla_put_failure;

            if (wpa_driver_nl80211_scan(bss, params)) {
                wpa_driver_nl80211_set_mode(bss, drv->nlmode);
                goto nla_put_failure;
            }

            /* Restore AP mode when processing scan results */
            drv->ap_scan_as_station = drv->nlmode;
            ret = 0;
        } else
            goto nla_put_failure;
#else /* HOSTAPD */
        goto nla_put_failure;
#endif /* HOSTAPD */
    }

    /* Not all drivers generate "scan completed" wireless event, so try to
     * read results after a timeout. */
    timeout = 10;
    if (drv->scan_complete_events) {
        /*
         * The driver seems to deliver events to notify when scan is
         * complete, so use longer timeout to avoid race conditions
         * with scanning and following association request.
         */
        timeout = 30;
    }
    wpa_printf(MSG_DEBUG, "Scan requested (ret=%d) - scan timeout %d "
           "seconds", ret, timeout);
    eloop_cancel_timeout(wpa_driver_nl80211_scan_timeout, drv, drv->ctx);
    eloop_register_timeout(timeout, 0, wpa_driver_nl80211_scan_timeout,
                   drv, drv->ctx);

nla_put_failure:
    nlmsg_free(msg);
    return ret;
}










static void send_scan_event(struct wpa_driver_nl80211_data *drv, int aborted,
                struct nlattr *tb[])
{
    driver_nl80211_log("----------");
    union wpa_event_data event;
    struct nlattr *nl;
    int rem;
    struct scan_info *info;
#define MAX_REPORT_FREQS 50
    int freqs[MAX_REPORT_FREQS];
    int num_freqs = 0;

    if (drv->scan_for_auth) {
        drv->scan_for_auth = 0;
        wpa_printf(MSG_DEBUG, "nl80211: Scan results for missing "
               "cfg80211 BSS entry");
        wpa_driver_nl80211_authenticate_retry(drv);
        return;
    }

    os_memset(&event, 0, sizeof(event));
    info = &event.scan_info;
    info->aborted = aborted;

    if (tb[NL80211_ATTR_SCAN_SSIDS]) {
        nla_for_each_nested(nl, tb[NL80211_ATTR_SCAN_SSIDS], rem) {
            struct wpa_driver_scan_ssid *s =
                &info->ssids[info->num_ssids];
            s->ssid = nla_data(nl);
            s->ssid_len = nla_len(nl);
            info->num_ssids++;
            if (info->num_ssids == WPAS_MAX_SCAN_SSIDS)
                break;
        }
    }
    if (tb[NL80211_ATTR_SCAN_FREQUENCIES]) {
        nla_for_each_nested(nl, tb[NL80211_ATTR_SCAN_FREQUENCIES], rem)
        {
            freqs[num_freqs] = nla_get_u32(nl);
            num_freqs++;
            if (num_freqs == MAX_REPORT_FREQS - 1)
                break;
        }
        info->freqs = freqs;
        info->num_freqs = num_freqs;
    }
    wpa_supplicant_event(drv->ctx, EVENT_SCAN_RESULTS, &event);
}







static int process_drv_event(struct nl_msg *msg, void *arg)
{
    driver_nl80211_log("----------");
    struct wpa_driver_nl80211_data *drv = arg;
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    struct i802_bss *bss;
    int ifidx = -1;

    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
          genlmsg_attrlen(gnlh, 0), NULL);

    if (tb[NL80211_ATTR_IFINDEX]) {
        ifidx = nla_get_u32(tb[NL80211_ATTR_IFINDEX]);

        for (bss = &drv->first_bss; bss; bss = bss->next)
            if (ifidx == -1 || ifidx == bss->ifindex) {
                printf("----------------------process_drv_event : 1--------------------------\n");
                do_process_drv_event(bss, gnlh->cmd, tb);
                return NL_SKIP;
            }
        wpa_printf(MSG_DEBUG,
               "nl80211: Ignored event (cmd=%d) for foreign interface (ifindex %d)",
               gnlh->cmd, ifidx);
    } else if (tb[NL80211_ATTR_WDEV]) {
        u64 wdev_id = nla_get_u64(tb[NL80211_ATTR_WDEV]);
        wpa_printf(MSG_DEBUG, "nl80211: Process event on P2P device");
        for (bss = &drv->first_bss; bss; bss = bss->next) {
            if (bss->wdev_id_set && wdev_id == bss->wdev_id) {
                printf("-----------------------process_drv_event : 2-------------------------\n");
                do_process_drv_event(bss, gnlh->cmd, tb);
                return NL_SKIP;
            }
        }
        wpa_printf(MSG_DEBUG,
               "nl80211: Ignored event (cmd=%d) for foreign interface (wdev 0x%llx)",
               gnlh->cmd, (long long unsigned int) wdev_id);
    }

    return NL_SKIP;
}





static int process_global_event(struct nl_msg *msg, void *arg)
{
    driver_nl80211_log("----------");
    struct nl80211_global *global = arg;
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    struct wpa_driver_nl80211_data *drv, *tmp;
    int ifidx = -1;
    struct i802_bss *bss;
    u64 wdev_id = 0;
    int wdev_id_set = 0;

    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
          genlmsg_attrlen(gnlh, 0), NULL);

    if (tb[NL80211_ATTR_IFINDEX])
        ifidx = nla_get_u32(tb[NL80211_ATTR_IFINDEX]);
    else if (tb[NL80211_ATTR_WDEV]) {
        wdev_id = nla_get_u64(tb[NL80211_ATTR_WDEV]);
        wdev_id_set = 1;
    }

    dl_list_for_each_safe(drv, tmp, &global->interfaces,
                  struct wpa_driver_nl80211_data, list) {
        for (bss = &drv->first_bss; bss; bss = bss->next) {
            if ((ifidx == -1 && !wdev_id_set) ||
                ifidx == bss->ifindex ||
                (wdev_id_set && bss->wdev_id_set &&
                 wdev_id == bss->wdev_id)) {
                printf("---------------------process_global_event---------------------------\n");
                do_process_drv_event(bss, gnlh->cmd, tb);
                return NL_SKIP;
            }
        }
    }

    return NL_SKIP;
}







static int wpa_driver_nl80211_init_nl_global(struct nl80211_global *global)
{
    driver_nl80211_log("----------");
    int ret;

    //创建一个 netlink 回调对象
    global->nl_cb = nl_cb_alloc(NL_CB_DEFAULT);
    if (global->nl_cb == NULL) {
        wpa_printf(MSG_ERROR, "nl80211: Failed to allocate netlink "
               "callbacks");
        return -1;
    }

    //nl_create_handle 返回值的类型为 nl_handle *
    //nl_create_handle 内部调用 genl_connect 连接到内核对应的模块。该函数最后的字符串参数("nl")仅用于输出调试信息
    global->nl = nl_create_handle(global->nl_cb, "nl");
    if (global->nl == NULL)
        goto err;

    //向 netlink 中的 "nl" 模块查询 "nl80211" 模块的编号。 genl_ctrl_resolve 函数本是由 libnl 定义，但在 driver_nl80211.c 中通过
    //#define genl_ctrl_resolve android_genl_ctrl_resolve
    //将其指向 android_genl_ctrl_resolve。该函数内部通过发送查询消息来获取 "nl80211" 模块的 family 值。
    global->nl80211_id = genl_ctrl_resolve(global->nl, "nl80211");
    if (global->nl80211_id < 0) {
        wpa_printf(MSG_ERROR, "nl80211: 'nl80211' generic netlink not "
               "found");
        goto err;
    }

    //创建另一个 nl_handle 来接收 netlink 消息
    global->nl_event = nl_create_handle(global->nl_cb, "event");
    if (global->nl_event == NULL)
        goto err;

    //先从 nl80211 模块中获得对应的组播组编号，如"scan"、"mlme"以及"regulatory"组播组的编号。
    ret = nl_get_multicast_id(global, "nl80211", "scan");
    if (ret >= 0)
        //加入某个组播组。这样，当某个组播有消息发送时， nl_event 就能收到了
        ret = nl_socket_add_membership(global->nl_event, ret);
    if (ret < 0) {
        wpa_printf(MSG_ERROR, "nl80211: Could not add multicast "
               "membership for scan events: %d (%s)",
               ret, strerror(-ret));
        goto err;
    }

    ret = nl_get_multicast_id(global, "nl80211", "mlme");
    if (ret >= 0)
        ret = nl_socket_add_membership(global->nl_event, ret);
    if (ret < 0) {
        wpa_printf(MSG_ERROR, "nl80211: Could not add multicast "
               "membership for mlme events: %d (%s)",
               ret, strerror(-ret));
        goto err;
    }

    ret = nl_get_multicast_id(global, "nl80211", "regulatory");
    if (ret >= 0)
        ret = nl_socket_add_membership(global->nl_event, ret);
    if (ret < 0) {
        wpa_printf(MSG_DEBUG, "nl80211: Could not add multicast "
               "membership for regulatory events: %d (%s)",
               ret, strerror(-ret));
        /* Continue without regulatory events */
    }

    nl_cb_set(global->nl_cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM,
          no_seq_check, NULL);                      //设置序列号检查函数为 no_seq_check
    nl_cb_set(global->nl_cb, NL_CB_VALID, NL_CB_CUSTOM,
          process_global_event, global);            //设置 netlink 消息回调处理函数

    //将 nl_event 对应的 socket 注册到 eloop 中，回调函数为 wpa_driver_nl80211_event_receive ，该函数内部将调用 nl_recv_msg ，而 nl_recv_msg 又会调用 process_global_event 。
    eloop_register_read_sock(nl_socket_get_fd(global->nl_event),
                 wpa_driver_nl80211_event_receive,
                 global->nl_cb, global->nl_event);

    return 0;

err:
    nl_destroy_handles(&global->nl_event);
    nl_destroy_handles(&global->nl);
    nl_cb_put(global->nl_cb);
    global->nl_cb = NULL;
    return -1;
}




static int wpa_driver_nl80211_scan(struct i802_bss *bss,
                   struct wpa_driver_scan_params *params)
{
    driver_nl80211_log("----------");
    printf("--------------------wpa_driver_nl80211_scan----------------------\n");
    struct wpa_driver_nl80211_data *drv = bss->drv;
    int ret = -1, timeout;
    struct nl_msg *msg = NULL;

    wpa_dbg(drv->ctx, MSG_DEBUG, "nl80211: scan request");
    drv->scan_for_auth = 0;

    msg = nl80211_scan_common(drv, NL80211_CMD_TRIGGER_SCAN, params,
                  bss->wdev_id_set ? &bss->wdev_id : NULL);
    if (!msg)
        return -1;

    if (params->p2p_probe) {
        struct nlattr *rates;

        wpa_printf(MSG_DEBUG, "nl80211: P2P probe - mask SuppRates");

        rates = nla_nest_start(msg, NL80211_ATTR_SCAN_SUPP_RATES);
        if (rates == NULL)
            goto nla_put_failure;

        /*
         * Remove 2.4 GHz rates 1, 2, 5.5, 11 Mbps from supported rates
         * by masking out everything else apart from the OFDM rates 6,
         * 9, 12, 18, 24, 36, 48, 54 Mbps from non-MCS rates. All 5 GHz
         * rates are left enabled.
         */
        NLA_PUT(msg, NL80211_BAND_2GHZ, 8,
            "\x0c\x12\x18\x24\x30\x48\x60\x6c");
        nla_nest_end(msg, rates);

        NLA_PUT_FLAG(msg, NL80211_ATTR_TX_NO_CCK_RATE);
    }

    ret = send_and_recv_msgs(drv, msg, NULL, NULL);
    msg = NULL;
    if (ret) {
        wpa_printf(MSG_DEBUG, "nl80211: Scan trigger failed: ret=%d "
               "(%s)", ret, strerror(-ret));
#ifdef HOSTAPD
        if (is_ap_interface(drv->nlmode)) {
            /*
             * mac80211 does not allow scan requests in AP mode, so
             * try to do this in station mode.
             */
            if (wpa_driver_nl80211_set_mode(
                    bss, NL80211_IFTYPE_STATION))
                goto nla_put_failure;

            if (wpa_driver_nl80211_scan(bss, params)) {
                wpa_driver_nl80211_set_mode(bss, drv->nlmode);
                goto nla_put_failure;
            }

            /* Restore AP mode when processing scan results */
            drv->ap_scan_as_station = drv->nlmode;
            ret = 0;
        } else
            goto nla_put_failure;
#else /* HOSTAPD */
        goto nla_put_failure;
#endif /* HOSTAPD */
    }

    /* Not all drivers generate "scan completed" wireless event, so try to
     * read results after a timeout. */
    timeout = 10;
    if (drv->scan_complete_events) {
        /*
         * The driver seems to deliver events to notify when scan is
         * complete, so use longer timeout to avoid race conditions
         * with scanning and following association request.
         */
        timeout = 30;
    }
    wpa_printf(MSG_DEBUG, "Scan requested (ret=%d) - scan timeout %d "
           "seconds", ret, timeout);
    eloop_cancel_timeout(wpa_driver_nl80211_scan_timeout, drv, drv->ctx);
    eloop_register_timeout(timeout, 0, wpa_driver_nl80211_scan_timeout,
                   drv, drv->ctx);

nla_put_failure:
    nlmsg_free(msg);
    return ret;
}





static struct nl_msg *
nl80211_scan_common(struct wpa_driver_nl80211_data *drv, u8 cmd,
            struct wpa_driver_scan_params *params, u64 *wdev_id)
{
    driver_nl80211_log("----------");
    printf("-----------------nl80211_scan_common-------------------\n");
    struct nl_msg *msg;
    size_t i;

    msg = nlmsg_alloc();
    if (!msg)
        return NULL;

    nl80211_cmd(drv, msg, 0, cmd);

    if (!wdev_id)
        NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, drv->ifindex);
    else
        NLA_PUT_U64(msg, NL80211_ATTR_WDEV, *wdev_id);

    if (params->num_ssids) {
        struct nlattr *ssids;

        ssids = nla_nest_start(msg, NL80211_ATTR_SCAN_SSIDS);
        if (ssids == NULL)
            goto fail;
        for (i = 0; i < params->num_ssids; i++) {
            wpa_hexdump_ascii(MSG_MSGDUMP, "nl80211: Scan SSID",
                      params->ssids[i].ssid,
                      params->ssids[i].ssid_len);
            if (nla_put(msg, i + 1, params->ssids[i].ssid_len,
                    params->ssids[i].ssid) < 0)
                goto fail;
        }
        nla_nest_end(msg, ssids);
    }

    if (params->extra_ies) {
        wpa_hexdump(MSG_MSGDUMP, "nl80211: Scan extra IEs",
                params->extra_ies, params->extra_ies_len);
        if (nla_put(msg, NL80211_ATTR_IE, params->extra_ies_len,
                params->extra_ies) < 0)
            goto fail;
    }

    if (params->freqs) {
        struct nlattr *freqs;
        freqs = nla_nest_start(msg, NL80211_ATTR_SCAN_FREQUENCIES);
        if (freqs == NULL)
            goto fail;
        for (i = 0; params->freqs[i]; i++) {
            wpa_printf(MSG_MSGDUMP, "nl80211: Scan frequency %u "
                   "MHz", params->freqs[i]);
            if (nla_put_u32(msg, i + 1, params->freqs[i]) < 0)
                goto fail;
        }
        nla_nest_end(msg, freqs);
    }

    os_free(drv->filter_ssids);
    drv->filter_ssids = params->filter_ssids;
    params->filter_ssids = NULL;
    drv->num_filter_ssids = params->num_filter_ssids;

    return msg;

fail:
nla_put_failure:
    nlmsg_free(msg);
    return NULL;
}



static int send_and_recv(struct nl80211_global *global,
             struct nl_handle *nl_handle, struct nl_msg *msg,
             int (*valid_handler)(struct nl_msg *, void *),
             void *valid_data)
{
    driver_nl80211_log("----------");
    struct nl_cb *cb;
    int err = -ENOMEM;

    cb = nl_cb_clone(global->nl_cb);
    if (!cb)
        goto out;

    err = nl_send_auto_complete(nl_handle, msg);
    if (err < 0)
        goto out;

    err = 1;

    nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
    nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
    nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

    if (valid_handler)
        nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM,
              valid_handler, valid_data);

    while (err > 0)
        nl_recvmsgs(nl_handle, cb);
 out:
    nl_cb_put(cb);
    nlmsg_free(msg);
    return err;
}




































