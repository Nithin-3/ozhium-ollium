/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * log.h - Logging module
 */
#pragma once

typedef enum { logError,
	       logWarn,
	       logInfo,
	       logDebug } logLevel;

void logInit(const char *filepath, logLevel level, int useSyslog);
void logSetLevel(logLevel level);
void logInitFromConfig(void);
void logWrite(logLevel level, const char *file, int line, const char *func, const char *fmt, ...);
int logGetFd(void);
void logClose(void);

#define logError(...) logWrite(logError, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define logWarn(...) logWrite(logWarn, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define logInfo(...) logWrite(logInfo, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define logDebug(...) logWrite(logDebug, __FILE__, __LINE__, __func__, __VA_ARGS__)
