/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * battery.c - Battery monitoring
 */

#include "daemon/utils/battery.h"
#include "shared/common.h"
#include "daemon/utils/tool.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>

static char bat_sta_path[PATH_MAX] = {0};
static char bat_cap_path[PATH_MAX] = {0};

int getBatteryPaths(char *cap, char *sta, size_t sz) {
    DIR *d = opendir("/sys/class/power_supply/");
    if (!d) return -1;
    struct dirent *de;
    while ((de = readdir(d))) {
        if (strncmp(de->d_name, "BAT", 3) != 0) continue;
	if (cap) snprintf(cap, sz, "/sys/class/power_supply/%s/capacity", de->d_name);
	if (sta) snprintf(sta, sz, "/sys/class/power_supply/%s/status", de->d_name);
        closedir(d);
        return 0;
    }
    closedir(d);
    return 1;
}

int getBattery(textData *t) {
	if (bat_sta_path[0] == '\0') {
		if (getBatteryPaths(bat_cap_path, bat_sta_path, PATH_MAX))
			return 1;
	}

	int cap;
	char buff[32];
	if (!catFile(bat_cap_path, &cap)) return 1;
	if (!catFileStr(bat_sta_path, buff, sizeof(buff))) return 1;

	snprintf(t->text, sizeof(t->text), "%d%%", cap);

	if (strcmp(buff, "Charging") == 0) t->action = BAT_CHARGE;
	else if (cap < 11) t->action = BAT_CRIT;
	else if (cap < 21) t->action = BAT_LOW;
	else if (strcmp(buff, "Discharging") == 0) t->action = BAT_DISCHARGE;
	else if (strcmp(buff, "Full") == 0) t->action = BAT_FULL;
	else t->action = BAT_IDEL;

	return 0;
}
