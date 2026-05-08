/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * ozhium-ollium.c - Main daemon entry point
 */

#include "daemon/monitors/inotify.h"
#include "daemon/monitors/netlink.h"
#include "daemon/monitors/pulse.h"
#include <stdio.h>
#include <unistd.h>

// Main entry point - initializes inotify, netlink, and PulseAudio then runs
// mainloop
int main() {
	fprintf(stdout, "ozhium-ollium started ....\n");
	fflush(stdout);
	pa_mainloop *ml = pa_mainloop_new();
	if (!ml) {
		fprintf(stderr, "Failed to create mainloop\n");
		return 1;
	}
	pa_mainloop_api *api = pa_mainloop_get_api(ml);

	if (initInotify(api) != 0) {
		fprintf(stderr, "Failed to init inotify\n");
		pa_mainloop_free(ml);
		return 1;
	}

	if (initNetlink(api) != 0) {
		fprintf(stderr, "Failed to init netlink\n");
		fflush(stderr);
	}

	if (initPulseAudio(api) != 0) {
		fprintf(stderr, "Failed to init PulseAudio\n");
		cleanupInotify();
		pa_mainloop_free(ml);
		return 1;
	}

	fprintf(stdout, "watch pulse sink source\n(Ctrl+C to exit)\n");
	fflush(stdout);
	pa_mainloop_run(ml, NULL);

	if (pa_ctx)
		pa_context_unref(pa_ctx);
	cleanupInotify();
	cleanupNetlink();
	pa_mainloop_free(ml);
	return 0;
}
