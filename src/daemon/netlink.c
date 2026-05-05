/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * ozhium-ollium.c - Main daemon entry point
 */

#include "daemon/netlink.h"
#include "daemon/battery.h"
#include "daemon/invoke.h"
#include "shared/common.h"
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

static int netlink_fd = -1;
static int uevent_fd  = -1;
static pa_io_event *netlink_io = NULL;
static pa_io_event *uevent_io  = NULL;
static pa_mainloop_api *netlink_api = NULL;

static char last_net_state[128] = {0};
static char last_bt_state[32] = {0};
static TEXT_ACTION last_battery_state;


static void netlink_recv(int fd) {
	char buf[8192];
	struct sockaddr_nl addr;
	socklen_t addr_len = sizeof(addr);

	ssize_t len = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&addr, &addr_len);
	if (len <= 0) return;

	struct nlmsghdr *nlh = (struct nlmsghdr *)buf;
	while (NLMSG_OK(nlh, (size_t)len)) {
		if (nlh->nlmsg_type == RTM_NEWLINK || nlh->nlmsg_type == RTM_DELLINK) {
			struct ifinfomsg *ifi = NLMSG_DATA(nlh);
			struct rtattr    *rta = IFLA_RTA(ifi);
			int rtl = IFLA_PAYLOAD(nlh);

			while (RTA_OK(rta, rtl)) {
				if (rta->rta_type == IFLA_IFNAME) {
					char *ifname = (char *)RTA_DATA(rta);
					if (strncmp(ifname, "wlan", 4) == 0 || strncmp(ifname, "wlp",  3) == 0 || strncmp(ifname, "eth",  3) == 0 || strncmp(ifname, "en",   2) == 0) {

						textData t = {0};
						snprintf(t.text, sizeof(t.text), "%s %s", ifname, (ifi->ifi_flags & IFF_UP) ? "up" : "down");

						if (strcmp(last_net_state, t.text) != 0) {
							strncpy(last_net_state, t.text, sizeof(last_net_state) - 1);
							execUI(TEXT, &t);
							fprintf(stdout, "[netlink] net: %s\n", t.text);
						}
					}
				}
				rta = RTA_NEXT(rta, rtl);
			}
		}
		nlh = NLMSG_NEXT(nlh, len);
	}
}


static void uevent_recv(int fd) {
	char buf[8192];
	ssize_t len = read(fd, buf, sizeof(buf) - 1);
	if (len <= 0) return;
	buf[len] = '\0';

	char action[32]    = {0};
	char subsystem[32] = {0};

	char *p = buf;
	p += strlen(p) + 1;

	while (p < buf + len) {
		if (strncmp(p, "ACTION=",    7)  == 0) strncpy(action,    p + 7,  sizeof(action)    - 1);
		else if (strncmp(p, "SUBSYSTEM=", 10) == 0) strncpy(subsystem, p + 10, sizeof(subsystem) - 1);
		p += strlen(p) + 1;
	}

	fprintf(stdout, "[uevent] subsystem=%s action=%s\n", subsystem, action);

	if (strcmp(subsystem, "bluetooth") == 0) {
		textData t = {0};
		snprintf(t.text, sizeof(t.text), "bluetooth %s", action);

		if (strcmp(last_bt_state, t.text) != 0) {
			strncpy(last_bt_state, t.text, sizeof(last_bt_state) - 1);
			execUI(TEXT, &t);
			fprintf(stdout, "[uevent] bluetooth: %s\n", action);
		}

	} else if (strcmp(subsystem, "power_supply") == 0) {
		if (strcmp(action, "change") == 0) {
			textData t = {0};
			getBattery(&t);

			if (last_battery_state != t.action) {
				last_battery_state = t.action;
				execUI(TEXT, &t);
				fprintf(stdout, "[uevent] battery: %s\n", t.text);
			}
		}
	}
}


static void netlink_cb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api; (void)e; (void)ud;
	if (!(events & PA_IO_EVENT_INPUT)) return;
	netlink_recv(fd);
}

static void uevent_cb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api; (void)e; (void)ud;
	if (!(events & PA_IO_EVENT_INPUT)) return;
	uevent_recv(fd);
}


int initNetlink(pa_mainloop_api *api) {
	netlink_api = api;

	netlink_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (netlink_fd < 0) { perror("netlink socket"); return -1; }

	struct sockaddr_nl addr = {0};
	addr.nl_family = AF_NETLINK;
	addr.nl_groups = RTMGRP_LINK;
	if (bind(netlink_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("netlink bind");
		close(netlink_fd);
		netlink_fd = -1;
		return -1;
	}
	netlink_io = api->io_new(api, netlink_fd, PA_IO_EVENT_INPUT, netlink_cb, NULL);

	uevent_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
	if (uevent_fd < 0) { perror("uevent socket"); return -1; }

	struct sockaddr_nl uaddr = {0};
	uaddr.nl_family = AF_NETLINK;
	uaddr.nl_groups = 1;
	if (bind(uevent_fd, (struct sockaddr *)&uaddr, sizeof(uaddr)) < 0) {
		perror("uevent bind");
		close(uevent_fd);
		uevent_fd = -1;
		return -1;
	}
	uevent_io = api->io_new(api, uevent_fd, PA_IO_EVENT_INPUT, uevent_cb, NULL);

	fprintf(stdout, "[netlink] monitoring net, bluetooth, battery\n");
	return 0;
}


void cleanupNetlink(void) {
	if (netlink_api && netlink_io) { netlink_api->io_free(netlink_io); netlink_io = NULL; }
	if (netlink_api && uevent_io)  { netlink_api->io_free(uevent_io);  uevent_io  = NULL; }
	if (netlink_fd >= 0) { close(netlink_fd); netlink_fd = -1; }
	if (uevent_fd  >= 0) { close(uevent_fd);  uevent_fd  = -1; }
}
