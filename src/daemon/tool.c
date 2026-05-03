#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#include "daemon/tool.h"

const char *findSelfDir() {
	static char dir[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", dir, sizeof(dir) - 1);
	if (len == -1) return ".";
	dir[len] = '\0';
	char *slash = strrchr(dir, '/');
	if (slash) *slash = '\0';
	return dir;
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
    snprintf(fmt, sizeof(fmt), "%%%zus", maxLen - 1);
    int r = fscanf(f, fmt, v);
    fclose(f);
    return r == 1;
}

