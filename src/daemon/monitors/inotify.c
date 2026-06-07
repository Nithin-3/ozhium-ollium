/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * inotify.c - inotify monitoring for backlight changes
 */

#include "daemon/monitors/inotify.h"
#include "shared/log.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>

static int in_fd = -1;

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
		}
		p += sizeof(struct inotify_event) + ev->len;
	}
}

int initInotify(pa_mainloop_api *api) {
	in_fd = inotify_init();
	if (in_fd < 0) {
		logError("inotify init: %s", strerror(errno));
		return -1;
	}

	return 0;
}

void cleanupInotify(void) {
	if (in_fd >= 0) {
		close(in_fd);
		in_fd = -1;
	}
}
