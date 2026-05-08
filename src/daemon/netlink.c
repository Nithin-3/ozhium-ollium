/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * ozhium-ollium.c - netlink socket monitoring
 */

#include "daemon/netlink.h"
#include "daemon/battery.h"
#include "daemon/invoke.h"
#include "shared/common.h"
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_arp.h>
#include <linux/input.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#define MAX_INTERFACES 64


static int netlink_fd = -1;
static int uevent_fd  = -1;
static pa_io_event *netlink_io = NULL;
static pa_io_event *uevent_io  = NULL;
static pa_mainloop_api *netlink_api = NULL;
static uint32_t prev_flags[MAX_INTERFACES] = {0};

int is_wifi(const char *ifname) {
	char path[256];
	snprintf(path, sizeof(path), "/sys/class/net/%s/phy80211", ifname);
	struct stat st;
	return stat(path, &st) == 0;
}

void netlinkRecv(int fd) {
	char buf[8192];
	struct sockaddr_nl addr;
	socklen_t addr_len = sizeof(addr);
	ssize_t len = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&addr, &addr_len);
	if (len <= 0) return;
	struct nlmsghdr *nlh = (struct nlmsghdr *)buf;
	while (NLMSG_OK(nlh, len)) {
		if (nlh->nlmsg_type == RTM_NEWLINK || nlh->nlmsg_type == RTM_DELLINK) {
			struct ifinfomsg *ifi = NLMSG_DATA(nlh);
			int idx = ifi->ifi_index;
			if (idx < 0 || idx >= MAX_INTERFACES) {
				nlh = NLMSG_NEXT(nlh, len);
				continue;
			}
			uint32_t old_flags = prev_flags[idx];
			uint32_t new_flags = ifi->ifi_flags;
			uint32_t changed = old_flags ^ new_flags;
			prev_flags[idx] = new_flags;
			if (!(changed & (IFF_UP | IFF_RUNNING | IFF_LOWER_UP))) {
				nlh = NLMSG_NEXT(nlh, len);
				continue;
			}
			int was_connected = (old_flags & IFF_RUNNING) && (old_flags & IFF_LOWER_UP);
			int now_connected = (new_flags & IFF_RUNNING) && (new_flags & IFF_LOWER_UP);
			if (was_connected == now_connected) {
				nlh = NLMSG_NEXT(nlh, len);
				continue;
			}
			struct rtattr *rta = IFLA_RTA(ifi);
			int rtl = IFLA_PAYLOAD(nlh);
			while (RTA_OK(rta, rtl)) {
				if (rta->rta_type == IFLA_IFNAME) {
					char *ifname = (char *)RTA_DATA(rta);
					const char *state = now_connected ? "connected" : "disconnected";
					if (is_wifi(ifname)) {
						textData t = {0};
						t.action = WIFI;
						snprintf(t.text, sizeof(t.text), "%s %s", ifname, state);
						fprintf(stdout, "[netlink] net: %s\n", t.text);
						execUI(TEXT, &t);
					} else if (ifi->ifi_type == ARPHRD_ETHER) {
						textData t = {0};
						t.action = ETHERNET;
						snprintf(t.text, sizeof(t.text), "%s %s", ifname, state);
						fprintf(stdout, "[netlink] net: %s\n", t.text);
						execUI(TEXT, &t);
					}
				}
				rta = RTA_NEXT(rta, rtl);
			}
		}
		nlh = NLMSG_NEXT(nlh, len);
	}
}


// handle changes on bluetooth and battery
void ueventRecv(int fd) {
	char buf[8192];
	ssize_t len = read(fd, buf, sizeof(buf) - 1);
	if (len <= 0) return;
	buf[len] = '\0';

	char action[32] = {0};
	char subsystem[32] = {0};
	char power_supply_type[8] = {0};
	char name[128] = {0};
	unsigned int power_supply_capacity;
	struct input_id {
		unsigned int bus;
		unsigned int vendor;
		unsigned int product;
		unsigned int version;
	} product = {0};

	char *p = buf;
	p += strlen(p) + 1;

	while (p < buf + len) {
		if (strncmp(p, "ACTION=",    7)  == 0) strncpy(action,    p + 7,  sizeof(action) - 1);
		else if (strncmp(p, "SUBSYSTEM=", 10) == 0) strncpy(subsystem, p + 10, sizeof(subsystem) - 1);
		else if (strncmp(p, "DEVNAME=",  8)  == 0) strncpy(name,   p + 8,  sizeof(name) - 1);
		else if (strncmp(p, "NAME=",    5)  == 0) strncpy(name,      p + 5,  sizeof(name) - 1);
		else if (strncmp(p, "HID_NAME=",    9)  == 0) strncpy(name,      p + 9,  sizeof(name) - 1);
		else if (strncmp(p, "POWER_SUPPLY_TYPE=", 18) == 0) strncpy(power_supply_type, p + 18, sizeof(power_supply_type) - 1);
		else if (strncmp(p, "POWER_SUPPLY_CAPACITY=", 22) == 0) sscanf(p + 22, "%u", &power_supply_capacity);
		else if (strncmp(p, "PRODUCT=",    8)  == 0)
			sscanf(p + 8, "%x/%x/%x/%x", &product.bus, &product.vendor, &product.product, &product.version); // NOTE: input event -> bus/vendor/product/version : these are the value PRODUCT= have
		// fprintf(stdout, "%s\n",p);
		p += strlen(p) + 1;
	}
	// fprintf(stdout, "\n");

	// if (0 == strcmp(subsystem, "bluetooth")) {
	// 	textData t = {0};
	// 	t.action = BLUETOOTH;
	// 	snprintf(t.text, sizeof(t.text), "bluetooth %s", action);
	// 	execUI(TEXT, &t);
	// 	return;
	// }
	
	if (0 == strcmp(subsystem, "power_supply") && 0 == strcmp(action, "change") && 0 == strcmp(power_supply_type, "Battery")) {
		sleep(1);
		textData t = {0};
		static ACTION act = INVALID;
		getBattery(&t);
		if (t.action == act) return;
		act = t.action;
		execUI(TEXT, &t);
                return;
	}

	if (0 == strcmp(subsystem, "block") && name[0]) {
		fprintf(stdout, "[usb] device=%s %s\n",name, action);
		textData t = {0};
		t.action = INVALID;
		snprintf(t.text, sizeof(t.text), "[block] %s %s", name, action);
		execUI(TEXT, &t);
		return;
	}

	if (0 == strcmp(subsystem, "hid") && name[0]) {
		fprintf(stdout, "[hid] %s %s\n", name, action);
		textData t = {0};
		t.action = INVALID;
		snprintf(t.text, sizeof(t.text), "[HID] %s %s", name, action);
		execUI(TEXT, &t);
		return;
	}

	if (0 == strcmp(subsystem, "input") && name[0]) {
		switch (product.bus) {
			case BUS_BLUETOOTH: // bluetooth bus -> 0x05 '5'
				fprintf(stdout, "[input] %s %s\n", name, action);
				textData t = {0};
				t.action = BLUETOOTH;
				snprintf(t.text, sizeof(t.text), "%s %s", name, action);
				execUI(TEXT, &t);
				break;
			case 0: break;
			default:
				fprintf(stdout, "%d this bus not handle",product.bus);
		}
		return;
	}
}


void netlinkCb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api; (void)e; (void)ud;
	if (!(events & PA_IO_EVENT_INPUT)) return;
	netlinkRecv(fd);
}

void ueventCb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api; (void)e; (void)ud;
	if (!(events & PA_IO_EVENT_INPUT)) return;
	ueventRecv(fd);
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
	netlink_io = api->io_new(api, netlink_fd, PA_IO_EVENT_INPUT, netlinkCb, NULL);

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
	uevent_io = api->io_new(api, uevent_fd, PA_IO_EVENT_INPUT, ueventCb, NULL);

	fprintf(stdout, "[netlink] monitoring net, bluetooth, battery\n");
	return 0;
}


void cleanupNetlink(void) {
	if (netlink_api && netlink_io) { netlink_api->io_free(netlink_io); netlink_io = NULL; }
	if (netlink_api && uevent_io)  { netlink_api->io_free(uevent_io);  uevent_io  = NULL; }
	if (netlink_fd >= 0) { close(netlink_fd); netlink_fd = -1; }
	if (uevent_fd  >= 0) { close(uevent_fd);  uevent_fd  = -1; }
}
