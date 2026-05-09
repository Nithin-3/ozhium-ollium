/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * inotify.c - inotify monitoring for backlight changes
 */

#include "daemon/monitors/inotify.h"
#include "daemon/invoke.h"
#include "daemon/utils/backlight.h"
#include "shared/common.h"
#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>

static int in_fd = -1;
static int bri_wd = -1;

static void inotifyCb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api;
	(void)e;
	(void)ud;
	if (!(events & PA_IO_EVENT_INPUT))
		return;
	char buf[4096];
	ssize_t len = read(fd, buf, sizeof(buf));
	if (len <= 0)
		return;
	for (char *p = buf; p < buf + len;) {
		struct inotify_event *ev = (struct inotify_event *)p;
		if (ev->mask & IN_MODIFY) {
			if (ev->wd == bri_wd) {
				sliderData slider;
				if (0 == getBacklight(&slider))
					execUI(BACKLIGHT, &slider);
			}
		}
		p += sizeof(struct inotify_event) + ev->len;
	}
}

int initInotify(pa_mainloop_api *api) {
	in_fd = inotify_init();
	if (in_fd < 0) {
		perror("inotify init");
		return -1;
	}

	if (getBacklightPaths(bri_path, max_path, PATH_MAX)) {
		fprintf(stdout, "could not find backlight path\n");
		fflush(stdout);
	} else {
		bri_wd = inotify_add_watch(in_fd, bri_path, IN_MODIFY);
		if (bri_wd < 0) {
			perror("inotify watch");
			close(in_fd);
			in_fd = -1;
			return -1;
		}
		fprintf(stdout, "[inotify watch] path : %s\n", bri_path);
		fflush(stdout);
	}

	api->io_new(api, in_fd, PA_IO_EVENT_INPUT, inotifyCb, NULL);
	return 0;
}

void cleanupInotify(void) {
	if (in_fd >= 0) {
		close(in_fd);
		in_fd = -1;
	}
	bri_wd = -1;
}
