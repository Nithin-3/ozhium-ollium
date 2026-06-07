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

int getBacklight(sliderData *s, char *path) {
	char bri_path[strlen(path) + strlen("/brightness") + 1];
	char max_path[strlen(path) + strlen("/max_brightness") + 1];

	snprintf(bri_path, sizeof(bri_path), "%s/brightness", path);
	snprintf(max_path, sizeof(max_path), "%s/max_brightness", path);

	int crnt, max;
	if (!catFile(max_path, &max) || !catFile(bri_path, &crnt))
		return -1;
	s->min = 0.0f;
	s->max = 1.0f;
	s->current = (float)crnt / max;
	s->action = BACKLIGHT;
	return 0;
}
