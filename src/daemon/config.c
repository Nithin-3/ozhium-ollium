/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * config.c - Daemon configuration loader
 */

#include "daemon/config.h"
#include "daemon/utils/tool.h"
#include "ini.h"
#include "shared/common.h"
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>

struct cfgCtx {
	const char *section, *key;
	char *val;
};

static int configHandler(void *user, const char *section, const char *name, const char *value) {
	struct cfgCtx *ctx = user;

	char buf[32];
	snprintf(buf, sizeof(buf), "daemon.%s", ctx->section);

	if (strcmp(section, buf) == 0 && strcmp(name, ctx->key) == 0) {
		ctx->val = strdup(value);
		return 0;
	}

	return 1;
}

static const char *actionToName(ACTION a) {
	switch (a) {
		case BACKLIGHT:
			return "backlight";
		case AUDIO:
			return "audio";
		case AUDIO_MUTE:
			return "audio_mute";
		case MIC:
			return "mic";
		case MIC_MUTE:
			return "mic_mute";
		case BAT_CRIT:
			return "bat_crit";
		case BAT_LOW:
			return "bat_low";
		case BAT_FULL:
			return "bat_full";
		case BAT_CHARGE:
			return "bat_charge";
		case BAT_DISCHARGE:
			return "bat_discharge";
		case BAT_IDEL:
			return "bat_idel";
		case WIFI:
			return "wifi";
		case ETHERNET:
			return "ethernet";
		case BLUETOOTH:
			return "bluetooth";
		default:
			return "invalid";
	}
}

static void freeArgs(char **args, int n) {
	for (int i = 0; i < n; i++)
		free(args[i]);
	free(args);
}

static char **splitBySpace(const char *str) {
	if (!str)
		return NULL;

	// count tokens first
	int n = 0;
	char *tmp = strdup(str);
	if (!tmp)
		return NULL;
	for (char *t = strtok(tmp, " "); t; t = strtok(NULL, " "))
		n++;
	free(tmp);

	if (n == 0)
		return NULL;

	// allocate once, exact size
	char **args = malloc((n + 1) * sizeof(char *));
	if (!args)
		return NULL;

	tmp = strdup(str);
	if (!tmp) {
		free(args);
		return NULL;
	}

	int i = 0;
	for (char *t = strtok(tmp, " "); t; t = strtok(NULL, " ")) {
		args[i] = strdup(t);
		if (!args[i]) {
			freeArgs(args, i);
			free(tmp);
			return NULL;
		}
		i++;
	}
	args[n] = NULL;
	free(tmp);
	return args;
}

static char **buildArgs(const char **parts, int n) {
	char **args = malloc((n + 1) * sizeof(char *));
	if (!args)
		return NULL;
	for (int i = 0; i < n; i++) {
		args[i] = strdup(parts[i]);
		if (!args[i]) {
			freeArgs(args, i);
			return NULL;
		}
	}
	args[n] = NULL;
	return args;
}

char *daemonConfigGet(const char *section, const char *key) {
	char *path = findConfigPath("ozhium-ollium.conf");
	if (!path)
		return "";

	struct cfgCtx ctx = { .section = section, .key = key, .val = "" };
	ini_parse(path, configHandler, &ctx);
	return ctx.val;
}

char **daemonNativeExec(ACTION a, void *data) {
	const char *selfPath = findSelfDir();
	char uiBinary[PATH_MAX];
	snprintf(uiBinary, sizeof(uiBinary), "%s/ozhium-ollium-ui", selfPath);

	switch (a) {
		case BACKLIGHT:
		case AUDIO:
		case AUDIO_MUTE:
		case MIC:
		case MIC_MUTE: {
			sliderData *s = (sliderData *)data;
			char minBuf[32], maxBuf[32], curBuf[32], actionBuf[32];
			snprintf(minBuf, sizeof(minBuf), "%.6f", s->min);
			snprintf(maxBuf, sizeof(maxBuf), "%.6f", s->max);
			snprintf(curBuf, sizeof(curBuf), "%.6f", s->current);
			snprintf(actionBuf, sizeof(actionBuf), "%s", actionToString(s->action));
			const char *parts[] = { uiBinary, "--element", "slider", "--min", minBuf, "--max", maxBuf, "--current", curBuf, "--action", actionBuf };
			return buildArgs(parts, sizeof(parts) / sizeof(parts[0]));
		}

		case BAT_CRIT:
		case BAT_LOW:
		case BAT_FULL:
		case BAT_CHARGE:
		case BAT_DISCHARGE:
		case BAT_IDEL:
		case WIFI:
		case ETHERNET:
		case BLUETOOTH:
		case INVALID: {
			textData *t = (textData *)data;
			char actionBuf[32];
			snprintf(actionBuf, sizeof(actionBuf), "%s", actionToString(t->action));
			const char *parts[] = { uiBinary, "--element", "text", "--text", t->text, "--action", actionBuf };
			return buildArgs(parts, sizeof(parts) / sizeof(parts[0]));
		}
	}
	return NULL;
}

char **daemonExec(ACTION a, void *data) {
	char **cmd = splitBySpace(daemonConfigGet("exec", actionToName(a)));
	if (cmd && cmd[0])
		switch (a) {
			case BACKLIGHT:
			case AUDIO:
			case AUDIO_MUTE:
			case MIC:
			case MIC_MUTE: {
				sliderData *s = (sliderData *)data;
				char curBuf[32];
				snprintf(curBuf, sizeof(curBuf), "%.6f", s->current);
				for (int i = 1; cmd[i] != NULL; i++)
					cmd[i] = strReplace(cmd[i], "#val#", curBuf);
				return cmd;
			}

			case BAT_CRIT:
			case BAT_LOW:
			case BAT_FULL:
			case BAT_CHARGE:
			case BAT_DISCHARGE:
			case BAT_IDEL:
			case WIFI:
			case ETHERNET:
			case BLUETOOTH:
			case INVALID: {
				textData *t = (textData *)data;
				for (int i = 1; cmd[i] != NULL; i++)
					cmd[i] = strReplace(cmd[i], "#val#", t->text);
				return cmd;
			}
		}
	return daemonNativeExec(a, data);
}

int isDaemonNativeExec(ACTION a) {
	char *cmd = daemonConfigGet("exec", actionToName(a));
	if (cmd && cmd[0])
		return strcmp(daemonConfigGet("native.exec", actionToName(a)), "off") != 0;
	return 1;
}
