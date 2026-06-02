/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * log.c - Logging module implementation
 */

#include "shared/log.h"
#include "shared/common.h"
#include "ini.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <sys/syslog.h>

static FILE *logFile = NULL;
static logLevel currentLevel = logInfo;
static int useSyslogFlag = 0;

static const char *levelStr(logLevel level) {
	switch (level) {
		case logError:
			return "ERROR";
		case logWarn:
			return "WARN";
		case logInfo:
			return "INFO";
		case logDebug:
			return "DEBUG";
	}
	return "UNKNOWN";
}

static int syslogMap(logLevel level) {
	switch (level) {
		case logError:
			return LOG_ERR;
		case logWarn:
			return LOG_WARNING;
		case logInfo:
			return LOG_INFO;
		case logDebug:
			return LOG_DEBUG;
	}
	return LOG_INFO;
}

static logLevel parseLevel(const char *s) {
	if (strcasecmp(s, "error") == 0)
		return logError;
	if (strcasecmp(s, "warn") == 0)
		return logWarn;
	if (strcasecmp(s, "info") == 0)
		return logInfo;
	if (strcasecmp(s, "debug") == 0)
		return logDebug;
	return logInfo;
}

void logInit(const char *filepath, logLevel level, int useSyslog) {
	currentLevel = level;
	useSyslogFlag = useSyslog;

	if (filepath && filepath[0]) {
		logFile = fopen(filepath, "a");
		if (!logFile)
			fprintf(stderr, "[log] failed to open log file: %s\n", filepath);
	}

	if (useSyslogFlag) {
		openlog("ozhium-ollium", LOG_PID | LOG_NDELAY, LOG_USER);
	}
}

void logSetLevel(logLevel level) {
	currentLevel = level;
}

void logClose(void) {
	if (logFile) {
		fclose(logFile);
		logFile = NULL;
	}
	if (useSyslogFlag) {
		closelog();
		useSyslogFlag = 0;
	}
}

int logGetFd(void) {
	if (logFile)
		return fileno(logFile);
	return -1;
}

struct logCfgCtx {
	char *file;
	char *level;
	int syslog;
};

static int logConfigHandler(void *user, const char *section, const char *name, const char *value) {
	struct logCfgCtx *ctx = user;

	if (strcmp(section, "log") != 0)
		return 1;

	if (strcmp(name, "file") == 0) {
		free(ctx->file);
		ctx->file = strdup(value);
	} else if (strcmp(name, "level") == 0) {
		free(ctx->level);
		ctx->level = strdup(value);
	} else if (strcmp(name, "syslog") == 0) {
		ctx->syslog = strcmp(value, "true") == 0 || strcmp(value, "1") == 0;
	}
	return 1;
}

void logInitFromConfig(void) {
	char *path = findConfigPath("ozhium-ollium.conf");
	if (!path)
		return;

	struct logCfgCtx ctx = { .file = NULL, .level = NULL, .syslog = 0 };
	ini_parse(path, logConfigHandler, &ctx);

	logInit(ctx.file, ctx.level ? parseLevel(ctx.level) : logInfo, ctx.syslog);

	free(ctx.file);
	free(ctx.level);
}

void logWrite(logLevel level, const char *file, int line, const char *func, const char *fmt, ...) {
	if (level > currentLevel)
		return;

	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	char ts[32];
	strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", tm);

	va_list args, argsCopy;
	va_start(args, fmt);
	va_copy(argsCopy, args);

	FILE *out = logFile ? logFile : stderr;
	fprintf(out, "[%s] [%-5s] (%s:%d %s) ", ts, levelStr(level), file, line, func);
	vfprintf(out, fmt, args);
	fprintf(out, "\n");
	fflush(out);

	if (useSyslogFlag)
		vsyslog(syslogMap(level), fmt, argsCopy);

	va_end(argsCopy);
	va_end(args);
}
