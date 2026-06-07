/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * battery.c - Battery monitoring
 */

#include "daemon/utils/tool.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>


int findBatteryPath(char *buf, size_t len) {
	DIR *d = opendir("/sys/class/power_supply");
	if (!d)
		return 1;
	struct dirent *de;
	while ((de = readdir(d))) {
		if (de->d_name[0] == '.')
			continue;
		char typePath[PATH_MAX];
		snprintf(typePath, sizeof(typePath), "/sys/class/power_supply/%s/type", de->d_name);
		char type[16];
		if (catFileStr(typePath, type, sizeof(type)) && strcmp(type, "Battery") == 0) {
			snprintf(buf, len, "/sys/class/power_supply/%s", de->d_name);
			closedir(d);
			return 0;
		}
	}
	closedir(d);
	return 1;
}

int getBattery(textData *t, char *path) {

	char batCapPath[strlen(path) + strlen("/capacity") + 1];
	char batStaPath[strlen(path) + strlen("/status") + 1];

	snprintf(batCapPath, sizeof(batCapPath), "%s/capacity", path);
	snprintf(batStaPath, sizeof(batStaPath), "%s/status", path);

	int cap;
	char buff[32];
	if (!catFile(batCapPath, &cap))
		return 1;
	if (!catFileStr(batStaPath, buff, sizeof(buff)))
		return 1;

	snprintf(t->text, sizeof(t->text), "%d%%", cap);

	if (strcmp(buff, "Charging") == 0)
		t->action = BAT_CHARGE;
	else if (cap < 11)
		t->action = BAT_CRIT;
	else if (cap < 21)
		t->action = BAT_LOW;
	else if (strcmp(buff, "Discharging") == 0)
		t->action = BAT_DISCHARGE;
	else if (strcmp(buff, "Full") == 0)
		t->action = BAT_FULL;
	else
		t->action = BAT_IDEL;

	return 0;
}
