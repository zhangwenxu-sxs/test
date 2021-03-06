/*
 * WPA Supplicant - auto scan
 * Copyright (c) 2012, Intel Corporation. All rights reserved.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "config.h"
#include "wpa_supplicant_i.h"
#include "bss.h"
#include "scan.h"
#include "autoscan.h"

#ifdef CONFIG_AUTOSCAN_EXPONENTIAL
extern const struct autoscan_ops autoscan_exponential_ops;
#endif /* CONFIG_AUTOSCAN_EXPONENTIAL */

#ifdef CONFIG_AUTOSCAN_PERIODIC
extern const struct autoscan_ops autoscan_periodic_ops;
#endif /* CONFIG_AUTOSCAN_PERIODIC */

static const struct autoscan_ops * autoscan_modules[] = {
#ifdef CONFIG_AUTOSCAN_EXPONENTIAL
	&autoscan_exponential_ops,
#endif /* CONFIG_AUTOSCAN_EXPONENTIAL */
#ifdef CONFIG_AUTOSCAN_PERIODIC
	&autoscan_periodic_ops,
#endif /* CONFIG_AUTOSCAN_PERIODIC */
	NULL
};


static void request_scan(struct wpa_supplicant *wpa_s)
{
	wpa_s->scan_req = MANUAL_SCAN_REQ;

	printf("[ZWX] request_scan \n");
	if (wpa_supplicant_req_sched_scan(wpa_s))
		wpa_supplicant_req_scan(wpa_s, wpa_s->scan_interval, 0);
}


int autoscan_init(struct wpa_supplicant *wpa_s, int req_scan)
{
    //1

	const char *name = wpa_s->conf->autoscan;
	const char *params;
	size_t nlen;
	int i;
	const struct autoscan_ops *ops = NULL;

	if (wpa_s->autoscan && wpa_s->autoscan_priv)
		return 0;

	if (name == NULL)
		return 0;

	params = os_strchr(name, ':');
	if (params == NULL) {
		params = "";
		nlen = os_strlen(name);
	} else {
		nlen = params - name;
		params++;
	}

	for (i = 0; autoscan_modules[i]; i++) {
		if (os_strncmp(name, autoscan_modules[i]->name, nlen) == 0) {
		    printf("[WJ_DEBUG] autoscan: %s \n", name);
			ops = autoscan_modules[i];
			break;
		}
	}

	if (ops == NULL) {
		wpa_printf(MSG_ERROR, "autoscan: Could not find module "
			   "matching the parameter '%s'", name);
		return -1;
	}

	wpa_s->autoscan_params = NULL;

	wpa_s->autoscan_priv = ops->init(wpa_s, params);
	if (wpa_s->autoscan_priv == NULL)
		return -1;
	wpa_s->autoscan = ops;

	wpa_printf(MSG_DEBUG, "autoscan: Initialized module '%s' with "
		   "parameters '%s'", ops->name, params);
	if (!req_scan)
		return 0;

	/*
	 * Cancelling existing scan requests, if any.
	 */
	wpa_supplicant_cancel_sched_scan(wpa_s);
	wpa_supplicant_cancel_scan(wpa_s);

	/*
	 * Firing first scan, which will lead to call autoscan_notify_scan.
	 */
	 printf("[ZWX] autoscan_init \n");
	request_scan(wpa_s);

	return 0;
}


void autoscan_deinit(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->autoscan && wpa_s->autoscan_priv) {
		wpa_printf(MSG_DEBUG, "autoscan: Deinitializing module '%s'",
			   wpa_s->autoscan->name);
		wpa_s->autoscan->deinit(wpa_s->autoscan_priv);
		wpa_s->autoscan = NULL;
		wpa_s->autoscan_priv = NULL;

		wpa_s->scan_interval = 5;
		wpa_s->sched_scan_interval = 0;
	}
}


int autoscan_notify_scan(struct wpa_supplicant *wpa_s,
			 struct wpa_scan_results *scan_res)
{

	int interval;

	if (wpa_s->autoscan && wpa_s->autoscan_priv) {
		interval = wpa_s->autoscan->notify_scan(wpa_s->autoscan_priv,
							scan_res);

		if (interval <= 0)
			return -1;

		wpa_s->scan_interval = interval;
		wpa_s->sched_scan_interval = interval;

		printf("[ZWX] autoscan_notify_scan \n");
		request_scan(wpa_s);
	}

	return 0;
}
