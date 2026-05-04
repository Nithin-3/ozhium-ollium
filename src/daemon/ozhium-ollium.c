/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * ozhium-ollium.c - Main daemon entry point
 */

#include "daemon/backLight.h"
#include "daemon/battery.h"
#include "daemon/invoke.h"
#include "daemon/pulse.h"
#include <dirent.h>
#include <libudev.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>


int bri_wd=0;
static struct udev_monitor *udev_mon = NULL;
static char prev_bat_status[32] = {0};

// inotify callback - monitors backlight brightness changes
static void inotify_cb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api;
	(void)e;
	(void)ud;
	if (!(events & PA_IO_EVENT_INPUT)) return;
	char buf[4096];
	ssize_t len = read(fd, buf, sizeof(buf));
	if (len <= 0) return;
	for (char *p = buf; p < buf + len; ) {
		struct inotify_event *ev = (struct inotify_event *)p;
		if (ev->mask & IN_MODIFY) {
			if (ev->wd == bri_wd) {
				sliderData slider;
				if (0 == getBacklight(&slider)) execUI(SLIDER, &slider);
			}
		}
		p += sizeof(struct inotify_event) + ev->len;
	}
}

// udev callback - monitors battery status changes
static void udev_cb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api; (void)e; (void)fd; (void)events; (void)ud;

	if (!(events & PA_IO_EVENT_INPUT)) return;

	struct udev_device *dev = udev_monitor_receive_device(udev_mon);
	if (!dev) return;

	const char *power_type = udev_device_get_property_value(dev, "POWER_SUPPLY_TYPE");
	sleep(1);
	if (!power_type || strcmp(power_type, "Battery") == 0) {
		const char *action = udev_device_get_action(dev);
		const char *status = udev_device_get_sysattr_value(dev, "status");
		if (action && 0 == strcmp(action, "change")) {

			if (prev_bat_status[0] != '\0' && 0 == strcmp(status, prev_bat_status)) {
				udev_device_unref(dev);
				return;
			}

			strncpy(prev_bat_status, status, sizeof(prev_bat_status) - 1);
			prev_bat_status[sizeof(prev_bat_status) - 1] = '\0';

			textData t = {0};
			getBattery(&t);
			execUI(TEXT, &t);
		}
	}

        udev_device_unref(dev);
}



// Main entry point - initializes inotify, udev, and PulseAudio then runs mainloop
int main() {
	fprintf(stdout,"ozhium-ollium started ....\n");
	pa_mainloop *ml = pa_mainloop_new();
	if (!ml) { fprintf(stderr, "Failed to create mainloop\n"); return 1; }
	pa_mainloop_api *api = pa_mainloop_get_api(ml);

	int in_fd = inotify_init();
	if (in_fd < 0) { perror("inotify init"); pa_mainloop_free(ml); return 1; }

	if (findBacklightPaths(bri_path, max_path, PATH_MAX)) {
		fprintf(stdout, "could not find backlight path\n");
	}else {
	
		bri_wd = inotify_add_watch(in_fd, bri_path, IN_MODIFY);
		if (bri_wd < 0) { perror("inotify watch"); close(in_fd); pa_mainloop_free(ml); return 1; }
		fprintf(stdout,"[inotify watch] path : %s\n", bri_path);

	}

	api->io_new(api, in_fd, PA_IO_EVENT_INPUT, inotify_cb, NULL);

	struct udev *udev = udev_new();
	if (!udev) { fprintf(stderr, "Failed to create udev\n"); }
	else {
		udev_mon = udev_monitor_new_from_netlink(udev, "kernel");
		if (udev_mon) {
			udev_monitor_filter_add_match_subsystem_devtype(udev_mon, "power_supply", NULL);
			udev_monitor_enable_receiving(udev_mon);
			int udev_fd = udev_monitor_get_fd(udev_mon);
			fprintf(stdout,"[udev] monitoring power_supply\n");
			api->io_new(api, udev_fd, PA_IO_EVENT_INPUT, udev_cb, NULL);
		}
	}


	if (initPulseAudio(api) != 0) { // if it fail daemon won't start
		fprintf(stderr, "Failed to init PulseAudio\n");
		close(in_fd);
		pa_mainloop_free(ml);
		return 1;
	}

	fprintf(stdout,"watch pulse srink source\n(Ctrl+C to exit)\n");
	pa_mainloop_run(ml, NULL);

	if (pa_ctx) pa_context_unref(pa_ctx);
	close(in_fd);
	if (udev_mon) udev_monitor_unref(udev_mon);
	if (udev) udev_unref(udev);
	pa_mainloop_free(ml);
	return 0;
}
