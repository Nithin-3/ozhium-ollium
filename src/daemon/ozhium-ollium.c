/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * ozhium-ollium.c - Main daemon entry point
 */

#include "daemon/monitors/inotify.h"
#include "daemon/monitors/netlink.h"
#include "daemon/monitors/pulse.h"
#include "shared/log.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifndef VERSION
#define VERSION "1.0.0"
#endif

// Main entry point - initializes inotify, netlink, and PulseAudio then runs
// mainloop
int main(int argc, char *argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-V") == 0) {
			printf("ozhium-ollium v%s\n", VERSION);
			return 0;
		}
		fprintf(stderr, "Unknown argument: %s\n", argv[1]);
		fprintf(stderr, "Usage: ozhium-ollium [--version|-V]\n");
		return 1;
	}
	logInitFromConfig();
	logInfo("ozhium-ollium started");
	pa_mainloop *ml = pa_mainloop_new();
	if (!ml) {
		logError("Failed to create mainloop");
		return 1;
	}
	pa_mainloop_api *api = pa_mainloop_get_api(ml);

	if (initInotify(api) != 0) {
		logError("Failed to init inotify");
		pa_mainloop_free(ml);
		return 1;
	}

	if (initNetlink(api) != 0) {
		logError("Failed to init netlink");
	}

	if (initPulseAudio(api) != 0) {
		logError("Failed to init PulseAudio");
		cleanupInotify();
		pa_mainloop_free(ml);
		return 1;
	}

	logInfo("watch pulse sink source (Ctrl+C to exit)");
	pa_mainloop_run(ml, NULL);

	if (pa_ctx)
		pa_context_unref(pa_ctx);
	cleanupInotify();
	cleanupNetlink();
	pa_mainloop_free(ml);
	return 0;
}
