#ifndef BACKLIGHT_TOOL_H
#define BACKLIGHT_TOOL_H

#include "invoke.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static char bri_path[PATH_MAX] = {0};
static char max_path[PATH_MAX] = {0};

static int findBacklightPaths(char *bri_path, char *max_path, size_t size) {
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

static int catFile(const char *p, int *v) {
    FILE *f = fopen(p, "r");
    if (!f) return 0;
    int r = fscanf(f, "%d", v);
    fclose(f);
    return r == 1;
}


static int getBacklight(sliderData *s) { //NOLINT
    if (bri_path[0] == '\0') {
        if (findBacklightPaths(bri_path, max_path, sizeof(bri_path)))
            return 1;
    }
    int crnt, max;
    if (!catFile(max_path, &max) || !catFile(bri_path, &crnt)) return -1;
    s->min = 0.0f;
    s->max = 1.0f;
    s->current =  (float)crnt / max;
    s->action = BACKLIGHT;
    return 0;
}

#endif
