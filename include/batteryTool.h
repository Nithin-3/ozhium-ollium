#ifndef BATTERY_TOOL_H
#define BATTERY_TOOL_H

#include "common.h"
#include "tool.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>

static char batSta_path[PATH_MAX] = {0};
static char batCap_path[PATH_MAX] = {0};

static int getBattery(textData *t) { // NOLINT
	if (batSta_path[0] == '\0') {
		if (findBatteryPaths(batCap_path, batSta_path, PATH_MAX))
			return 1;
	}

	int cap;
	char buff[32];
	if (!catFile(batCap_path, &cap)) return 1;
	if (!catFileStr(batSta_path, buff, sizeof(buff))) return 1;

	snprintf(t->text, sizeof(t->text), "%d", cap);

	if (strcmp(buff, "Charging") == 0) t->action = BAT_CHARGE;
	else if (strcmp(buff, "Discharging") == 0) t->action = BAT_DISCHARGE;
	else if (strcmp(buff, "Full") == 0) t->action = BAT_FULL;
	else if (cap < 21) t->action = BAT_LOW;
	else t->action = BAT_IDEL;

	return 0;
}

#endif
