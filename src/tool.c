#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#include "tool.h"

char *findConfigPath(const char *filename) {
	static char path[PATH_MAX];
	const char *home = getenv("HOME");

	if (home) {
		snprintf(path, PATH_MAX, "%s/.config/echo-meter/%s", home, filename);
		if (access(path, R_OK) == 0)
			return path;
	}

	snprintf(path, PATH_MAX, "/usr/share/echo-meter/%s", filename);
	if (access(path, R_OK) == 0)
		return path;

	return NULL;
}

const char *findSelfDir() {
	static char dir[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", dir, sizeof(dir) - 1);
	if (len == -1) return ".";
	dir[len] = '\0';
	// strip trailing binary name
	char *slash = strrchr(dir, '/');
	if (slash) *slash = '\0';
	return dir;
}
