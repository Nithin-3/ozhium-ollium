/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * config.c - Configuration loading
 */

#include "ui/config.h"
#include "shared/common.h"
#include "ini.h"
#include <string.h>
#include <stdlib.h>

AppConfig appConfig = {
	.horizontalAlign = "center",
	.verticalAlign = "top",
	.orientation = "horizontal",
	.timeOut = 3,
	.x = -1,
	.y = -1,
	.margin = 0,
};

Icons icons = {
	.backlight = "",
	.volume = " ",
	.volumeMute = "󰖁",
	.mic = "󰍬",
	.micMute = " ",
	.batCharge = "󰂄",
	.batDischarge = "󰁾",
	.batFull = " ",
	.batLow = "󱊡",
	.batIdel = "󰁹",
	.batCrit = "󰂃",
	.wifi = " ",
	.ethernet = "󰈀 ",
	.bluetooth = "",
};

SliderConfig sliderConfig = {
	.invert_dir = false,
	.label1 = "#ico#",
	.label2 = "#val#",
};

TextConfig textConfig = {
	.label = "#ico# #val#",
};

static int configHandler(void *user, const char *section, const char *name, const char *value) {
	(void)user;

	if (strcmp(section, "osd") == 0) {
		if (strcmp(name, "vertical-align") == 0) {
			if (strcmp(value, "top") == 0 || strcmp(value, "center") == 0 || strcmp(value, "bottom") == 0)
				appConfig.verticalAlign = strdup(value);
		} else if (strcmp(name, "horizontal-align") == 0) {
			if (strcmp(value, "left") == 0 || strcmp(value, "right") == 0 || strcmp(value, "center") == 0)
				appConfig.horizontalAlign = strdup(value);
		} else if (strcmp(name, "orientation") == 0) {
			if (strcmp(value, "vertical") == 0 || strcmp(value, "horizontal") == 0)
				appConfig.orientation = strdup(value);
		} else if (strcmp(name, "timeout") == 0) {
			int val = atoi(value);
			if (val > 0)
				appConfig.timeOut = val;
		} else if (strcmp(name, "x") == 0) {
			int val = atoi(value);
			if (val >= 0)
				appConfig.x = val;
		} else if (strcmp(name, "y") == 0) {
			int val = atoi(value);
			if (val >= 0)
				appConfig.y = val;
		} else if (strcmp(name, "margin") == 0) {
			int val = atoi(value);
			if (val >= 0)
				appConfig.margin = val;
		}
	} else if (strcmp(section, "slider") == 0) {
		if (strcmp(name, "invert-dir") == 0) {
			if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0)
				sliderConfig.invert_dir = true;
			else if (strcmp(value, "false") == 0 || strcmp(value, "0") == 0)
				sliderConfig.invert_dir = false;
		} else if (strcmp(name, "label1") == 0) {
			sliderConfig.label1 = strdup(value);
		} else if (strcmp(name, "label2") == 0) {
			sliderConfig.label2 = strdup(value);
		}
	} else if (strcmp(section, "text") == 0) {
		if (strcmp(name, "label") == 0)
			textConfig.label = strdup(value);
	} else if (strcmp(section, "icons") == 0) {
		if (strcmp(name, "backlight") == 0)
			icons.backlight = strdup(value);
		else if (strcmp(name, "volume") == 0)
			icons.volume = strdup(value);
		else if (strcmp(name, "volume-mute") == 0)
			icons.volumeMute = strdup(value);
		else if (strcmp(name, "mic") == 0)
			icons.mic = strdup(value);
		else if (strcmp(name, "mic-mute") == 0)
			icons.micMute = strdup(value);
		else if (strcmp(name, "bat-low") == 0)
			icons.batLow = strdup(value);
		else if (strcmp(name, "bat-full") == 0)
			icons.batFull = strdup(value);
		else if (strcmp(name, "bat-charge") == 0)
			icons.batCharge = strdup(value);
		else if (strcmp(name, "bat-discharge") == 0)
			icons.batDischarge = strdup(value);
		else if (strcmp(name, "bat-idel") == 0)
			icons.batIdel = strdup(value);
		else if (strcmp(name, "bat-crit") == 0)
			icons.batCrit = strdup(value);
		else if (strcmp(name, "wifi") == 0)
			icons.wifi = strdup(value);
		else if (strcmp(name, "ethernet") == 0)
			icons.ethernet = strdup(value);
		else if (strcmp(name, "bluetooth") == 0)
			icons.bluetooth = strdup(value);
	}

	return 1;
}

void loadConfig(void) {
	char *path = findConfigPath("ozhium-ollium.conf");
	if (path == NULL)
		return;
	ini_parse(path, configHandler, NULL);
}
