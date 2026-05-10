/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * backlight.c - Backlight monitoring
 */

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include "daemon/utils/backlight.h"
#include "daemon/utils/tool.h"

char bri_path[PATH_MAX] = { 0 };
char max_path[PATH_MAX] = { 0 };

int getBacklightPaths(char *bri_path, char *max_path, size_t size) {
	DIR *d = opendir("/sys/class/backlight/");
	if (!d)
		return 1;
	struct dirent *dir;
	while ((dir = readdir(d))) {
		if (dir->d_name[0] == '.')
			continue;
		snprintf(max_path, size, "/sys/class/backlight/%s/max_brightness", dir->d_name);
		snprintf(bri_path, size, "/sys/class/backlight/%s/brightness", dir->d_name);
		if (access(max_path, R_OK) == 0 && access(bri_path, R_OK) == 0) {
			closedir(d);
			return 0;
		}
	}
	closedir(d);
	return 1;
}

int getBacklight(sliderData *s) {
	if (bri_path[0] == '\0') {
		if (getBacklightPaths(bri_path, max_path, sizeof(bri_path)))
			return 1;
	}
	int crnt, max;
	if (!catFile(max_path, &max) || !catFile(bri_path, &crnt))
		return -1;
	s->min = 0.0f;
	s->max = 1.0f;
	s->current = (float)crnt / max;
	s->action = BACKLIGHT;
	return 0;
}
