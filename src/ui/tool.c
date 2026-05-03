#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#include "daemon/tool.h"

char *findConfigPath(const char *filename) {
	static char path[PATH_MAX];
	const char *home = getenv("HOME");

	if (home) {
		snprintf(path, PATH_MAX, "%s/.config/ozhium-ollium/%s", home, filename);
		if (access(path, R_OK) == 0)
			return path;
	}

	snprintf(path, PATH_MAX, "/usr/share/ozhium-ollium/%s", filename);
	if (access(path, R_OK) == 0)
		return path;

	return NULL;
}