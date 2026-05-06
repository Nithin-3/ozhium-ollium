/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * ozhium-ollium.c - Main daemon entry point
 */

#include "daemon/backLight.h"
#include "daemon/invoke.h"
#include "daemon/netlink.h"
#include "daemon/pulse.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>


int bri_wd=0;
static int mainloop_running = 0;

// inotify callback - monitors backlight and battery changes
static void inotify_cb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api; (void)e; (void)ud;
	if (!(events & PA_IO_EVENT_INPUT)) return;
	if (!mainloop_running) return;
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

// Main entry point - initializes inotify, netlink, and PulseAudio then runs mainloop
int main() {
	fprintf(stdout,"ozhium-ollium started ....\n");
	fflush(stdout);
	pa_mainloop *ml = pa_mainloop_new();
	if (!ml) { fprintf(stderr, "Failed to create mainloop\n"); return 1; }
	pa_mainloop_api *api = pa_mainloop_get_api(ml);

	mainloop_running = 1;

	int in_fd = inotify_init();
	if (in_fd < 0) { perror("inotify init"); pa_mainloop_free(ml); return 1; }

	if (findBacklightPaths(bri_path, max_path, PATH_MAX)) {
		fprintf(stdout, "could not find backlight path\n");
		fflush(stdout);
	}else {

		bri_wd = inotify_add_watch(in_fd, bri_path, IN_MODIFY);
		if (bri_wd < 0) { perror("inotify watch"); close(in_fd); pa_mainloop_free(ml); return 1; }
		fprintf(stdout,"[inotify watch] path : %s\n", bri_path);
		fflush(stdout);

	}

	api->io_new(api, in_fd, PA_IO_EVENT_INPUT, inotify_cb, NULL);

	if (initNetlink(api) != 0) {
		fprintf(stderr, "Failed to init netlink\n");
		fflush(stderr);
	}

	if (initPulseAudio(api) != 0) {
		fprintf(stderr, "Failed to init PulseAudio\n");
		close(in_fd);
		pa_mainloop_free(ml);
		return 1;
	}

	fprintf(stdout,"watch pulse sink source\n(Ctrl+C to exit)\n");
	fflush(stdout);
	pa_mainloop_run(ml, NULL);

	if (pa_ctx) pa_context_unref(pa_ctx);
	close(in_fd);
	cleanupNetlink();
	pa_mainloop_free(ml);
	return 0;
}
