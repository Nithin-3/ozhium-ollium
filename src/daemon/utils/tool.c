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
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include "daemon/utils/tool.h"

const char *findSelfDir() {
	static char dir[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", dir, sizeof(dir) - 1); // find the daemon execution path in your system
	if (len == -1) return ".";
	dir[len] = '\0';
	char *slash = strrchr(dir, '/');
	if (slash) *slash = '\0';
	return dir; // directory of this daemon
}

int catFile(const char *p, int *v) {
    FILE *f = fopen(p, "r");
    if (!f) return 0;
    int r = fscanf(f, "%d", v);
    fclose(f);
    return r == 1;
}

int catFileStr(const char *p, char *v, size_t maxLen) {
    FILE *f = fopen(p, "r");
    if (!f) return 0;
    char fmt[16];
    
    // Build a format string like "%Ns" where N = maxLen - 1
    // "%%" -> literal '%'
    // "%zu" -> prints size_t value (maxLen - 1)
    // "s" -> completes the format specifier for string input
    snprintf(fmt, sizeof(fmt), "%%%zus", maxLen - 1); // e.g: "%20s"
    int r = fscanf(f, fmt, v);  // e.g: read to 20 char
    fclose(f);
    return r == 1; // false on EOF or 0 (error)
}

