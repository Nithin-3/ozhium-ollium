#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>

#include "tool.h"

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

const char *findSelfDir() {
	static char dir[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", dir, sizeof(dir) - 1);
	if (len == -1) return ".";
	dir[len] = '\0';
	char *slash = strrchr(dir, '/');
	if (slash) *slash = '\0';
	return dir;
}


int findBacklightPaths(char *bri_path, char *max_path, size_t size) {
	DIR *d = opendir("/sys/class/backlight/");
	if (!d)
		return 1;
	struct dirent *dir;
	while ((dir = readdir(d))) {
		if (dir->d_name[0] == '.')
			continue;
		snprintf(max_path, size, "/sys/class/backlight/%s/max_brightness", dir->d_name);
		snprintf(bri_path, size, "/sys/class/backlight/%s/brightness", dir->d_name);
		if (access(max_path, R_OK) == 0 && access(bri_path, R_OK) == 0) {
			closedir(d);
			return 0;
		}
	}
	closedir(d);
	return 1;
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

