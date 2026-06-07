/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * netlink.c - netlink socket monitoring
 */

#include "daemon/monitors/netlink.h"
#include "daemon/utils/backlight.h"
#include "daemon/utils/battery.h"
#include "daemon/invoke.h"
#include "shared/common.h"
#include "shared/log.h"
#include <asm/types.h>
#include <errno.h>
#include <linux/limits.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_arp.h>
#include <linux/input.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAX_INTERFACES 64

static int netlink_fd = -1;
static int uevent_fd = -1;
static pa_io_event *netlink_io = NULL;
static pa_io_event *uevent_io = NULL;
static pa_mainloop_api *netlink_api = NULL;
static uint32_t prev_flags[MAX_INTERFACES] = { 0 };
static ACTION act = INVALID;

static int is_wifi(const char *ifname) {
	char path[256];
	snprintf(path, sizeof(path), "/sys/class/net/%s/phy80211", ifname);
	struct stat st;
	return stat(path, &st) == 0;
}

static int is_vpn_kind(const char *kind) {
	static const char *vpn_types[] = { "tun", "tap", "wireguard", "openvpn", "vti", "vti6", "gre", "gretap", "ipip", "sit", "ip6tnl", "ip6gre", "ip6gretap", "l2tp", "ppp" };
	for (size_t i = 0; i < sizeof(vpn_types) / sizeof(vpn_types[0]); i++)
		if (strcmp(kind, vpn_types[i]) == 0)
			return 1;
	return 0;
}

void netlinkRecv(int fd) {
	char buf[8192];
	struct sockaddr_nl addr;
	socklen_t addr_len = sizeof(addr);
	ssize_t len = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&addr, &addr_len);
	if (len <= 0)
		return;
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

			char *ifname = NULL;
			char *kind = NULL;
			struct rtattr *rta = IFLA_RTA(ifi);
			int rtl = IFLA_PAYLOAD(nlh);
			while (RTA_OK(rta, rtl)) {
				if (rta->rta_type == IFLA_IFNAME)
					ifname = (char *)RTA_DATA(rta);
				else if (rta->rta_type == IFLA_LINKINFO) {
					struct rtattr *rta_info = (struct rtattr *)RTA_DATA(rta);
					int rtl_info = RTA_PAYLOAD(rta);
					while (RTA_OK(rta_info, rtl_info)) {
						if (rta_info->rta_type == IFLA_INFO_KIND) {
							kind = (char *)RTA_DATA(rta_info);
							break;
						}
						rta_info = RTA_NEXT(rta_info, rtl_info);
					}
				}
				rta = RTA_NEXT(rta, rtl);
			}

			if (!ifname) {
				nlh = NLMSG_NEXT(nlh, len);
				continue;
			}

			const char *state = now_connected ? "connected" : "disconnected";
			textData t = { 0 };
			if (is_wifi(ifname)) {
				t.action = WIFI;
				snprintf(t.text, sizeof(t.text), "%s %s", ifname, state);
				logInfo("[netlink] net: %s", t.text);
				execUI(WIFI, &t);
			} else if (kind && is_vpn_kind(kind)) {
				t.action = VPN;
				snprintf(t.text, sizeof(t.text), "%s %s %s", kind, ifname, state);
				logInfo("[netlink] net: %s", t.text);
				execUI(VPN, &t);
			} else if (ifi->ifi_type == ARPHRD_ETHER) {
				t.action = ETHERNET;
				snprintf(t.text, sizeof(t.text), "%s %s", ifname, state);
				logInfo("[netlink] net: %s", t.text);
				execUI(ETHERNET, &t);
			}
		}
		nlh = NLMSG_NEXT(nlh, len);
	}
}

void ueventRecv(int fd) {
	char buf[8192];
	ssize_t len = read(fd, buf, sizeof(buf) - 1);
	if (len <= 0)
		return;
	buf[len] = '\0';

	char action[32] = { 0 };
	char subsystem[32] = { 0 };
	char power_supply_type[8] = { 0 };
	char name[128] = { 0 };
	char devPath[PATH_MAX] = { 0 };
	unsigned int power_supply_capacity;
	struct input_id {
		unsigned int bus;
		unsigned int vendor;
		unsigned int product;
		unsigned int version;
	} product = { 0 };

	char *p = buf;
	p += strlen(p) + 1;

	while (p < buf + len) {
		if (strncmp(p, "ACTION=", 7) == 0)
			strncpy(action, p + 7, sizeof(action) - 1);
		else if (strncmp(p, "SUBSYSTEM=", 10) == 0)
			strncpy(subsystem, p + 10, sizeof(subsystem) - 1);
		else if (strncmp(p, "DEVPATH=", 8) == 0)
			snprintf(devPath, sizeof(devPath), "/sys%s", p + 8);
		else if (strncmp(p, "DEVNAME=", 8) == 0)
			strncpy(name, p + 8, sizeof(name) - 1);
		else if (strncmp(p, "NAME=", 5) == 0)
			strncpy(name, p + 5, sizeof(name) - 1);
		else if (strncmp(p, "HID_NAME=", 9) == 0)
			strncpy(name, p + 9, sizeof(name) - 1);
		else if (strncmp(p, "POWER_SUPPLY_TYPE=", 18) == 0)
			strncpy(power_supply_type, p + 18, sizeof(power_supply_type) - 1);
		else if (strncmp(p, "POWER_SUPPLY_CAPACITY=", 22) == 0)
			sscanf(p + 22, "%u", &power_supply_capacity);
		else if (strncmp(p, "PRODUCT=", 8) == 0)
			sscanf(p + 8, "%x/%x/%x/%x", &product.bus, &product.vendor, &product.product, &product.version);  // NOTE: input event -> bus/vendor/product/version : these are the value PRODUCT= have
		// logDebug("%s", p);
		p += strlen(p) + 1;
	}
	// logDebug("");

	if (0 == strcmp(action, "change") && 0 == strcmp(subsystem, "backlight")) {
		sliderData s = { 0 };
		getBacklight(&s, devPath);
		execUI(BACKLIGHT, &s);
		return;
	}

	if (0 == strcmp(subsystem, "power_supply") && 0 == strcmp(action, "change") && 0 == strcmp(power_supply_type, "Battery")) {
		sleep(1);
		textData t = { 0 };
		getBattery(&t, devPath);
		if (t.action == act)
			return;
		act = t.action;
		execUI(t.action, &t);
		return;
	}

	if (0 == strcmp(subsystem, "block") && name[0]) {
		logInfo("[usb] device=%s %s", name, action);
		textData t = { 0 };
		t.action = BLOCK;
		snprintf(t.text, sizeof(t.text), "[block] %s %s", name, action);
		execUI(BLOCK, &t);
		return;
	}

	if (0 == strcmp(subsystem, "hid") && name[0]) {
		logInfo("[hid] %s %s", name, action);
		textData t = { 0 };
		t.action = HID;
		snprintf(t.text, sizeof(t.text), "[HID] %s %s", name, action);
		execUI(HID, &t);
		return;
	}

	if (0 == strcmp(subsystem, "input") && name[0]) {
		switch (product.bus) {
			case BUS_BLUETOOTH:  // bluetooth bus -> 0x05 '5'
				logInfo("[input] %s %s", name, action);
				textData t = { 0 };
				t.action = BLUETOOTH;
				snprintf(t.text, sizeof(t.text), "%s %s", name, action);
				execUI(BLUETOOTH, &t);
				break;
			case 0:
				break;
			default:
				logInfo("%d this bus not handled", product.bus);
		}
		return;
	}
}

void netlinkCb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api;
	(void)e;
	(void)ud;
	if (!(events & PA_IO_EVENT_INPUT))
		return;
	netlinkRecv(fd);
}

void ueventCb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api;
	(void)e;
	(void)ud;
	if (!(events & PA_IO_EVENT_INPUT))
		return;
	ueventRecv(fd);
}

int initNetlink(pa_mainloop_api *api) {
	netlink_api = api;

	netlink_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (netlink_fd < 0) {
		logError("netlink socket: %s", strerror(errno));
		return -1;
	}

	struct sockaddr_nl addr = { 0 };
	addr.nl_family = AF_NETLINK;
	addr.nl_groups = RTMGRP_LINK;
	if (bind(netlink_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		logError("netlink bind: %s", strerror(errno));
		close(netlink_fd);
		netlink_fd = -1;
		return -1;
	}
	netlink_io = api->io_new(api, netlink_fd, PA_IO_EVENT_INPUT, netlinkCb, NULL);

	uevent_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
	if (uevent_fd < 0) {
		logError("uevent socket: %s", strerror(errno));
		return -1;
	}

	struct sockaddr_nl uaddr = { 0 };
	uaddr.nl_family = AF_NETLINK;
	uaddr.nl_groups = 1;
	if (bind(uevent_fd, (struct sockaddr *)&uaddr, sizeof(uaddr)) < 0) {
		logError("uevent bind: %s", strerror(errno));
		close(uevent_fd);
		uevent_fd = -1;
		return -1;
	}
	char devPath[PATH_MAX] = { 0 };
	textData t = { 0 };
	if (findBatteryPath(devPath, sizeof(devPath)) == 0)
		getBattery(&t, devPath);
	act = t.action;

	uevent_io = api->io_new(api, uevent_fd, PA_IO_EVENT_INPUT, ueventCb, NULL);

	logInfo("[netlink] monitoring net, bluetooth, battery");
	return 0;
}

void cleanupNetlink(void) {
	if (netlink_api && netlink_io) {
		netlink_api->io_free(netlink_io);
		netlink_io = NULL;
	}
	if (netlink_api && uevent_io) {
		netlink_api->io_free(uevent_io);
		uevent_io = NULL;
	}
	if (netlink_fd >= 0) {
		close(netlink_fd);
		netlink_fd = -1;
	}
	if (uevent_fd >= 0) {
		close(uevent_fd);
		uevent_fd = -1;
	}
}
