/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * tool.c - Utility functions
 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include "daemon/utils/tool.h"
#include "shared/common.h"

const char *findSelfDir() {
	static char dir[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", dir, sizeof(dir) - 1);	 // find the daemon execution path in your system
	if (len == -1)
		return ".";
	dir[len] = '\0';
	char *slash = strrchr(dir, '/');
	if (slash)
		*slash = '\0';
	return dir;  // directory of this daemon
}

int catFile(const char *p, int *v) {
	FILE *f = fopen(p, "r");
	if (!f)
		return 0;
	int r = fscanf(f, "%d", v);
	fclose(f);
	return r == 1;
}

int catFileStr(const char *p, char *v, size_t maxLen) {
	FILE *f = fopen(p, "r");
	if (!f)
		return 0;
	char fmt[16];

	// Build a format string like "%Ns" where N = maxLen - 1
	// "%%" -> literal '%'
	// "%zu" -> prints size_t value (maxLen - 1)
	// "s" -> completes the format specifier for string input
	snprintf(fmt, sizeof(fmt), "%%%zus", maxLen - 1);  // e.g: "%20s"
	int r = fscanf(f, fmt, v);			   // e.g: read to 20 char
	fclose(f);
	return r == 1;	// false on EOF or 0 (error)
}

const char *actionToString(ACTION a) {
	switch (a) {
		case BACKLIGHT:
			return "s0";
		case AUDIO:
			return "s1";
		case AUDIO_MUTE:
			return "s2";
		case MIC:
			return "s3";
		case MIC_MUTE:
			return "s4";
		case BAT_LOW:
			return "t0";
		case BAT_FULL:
			return "t1";
		case BAT_CHARGE:
			return "t2";
		case BAT_DISCHARGE:
			return "t3";
		case BAT_IDEL:
			return "t4";
		case WIFI:
			return "t5";
		case ETHERNET:
			return "t6";
		case BLUETOOTH:
			return "t7";
		default:
			return "?";
	}
}

const char *elementToString(GUI_ELEMENT e) {
	switch (e) {
		case SLIDER:
			return "0";
		case TEXT:
			return "1";
		default:
			return "0";
	}
}
