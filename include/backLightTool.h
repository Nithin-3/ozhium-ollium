#ifndef BACKLIGHT_TOOL_H
#define BACKLIGHT_TOOL_H

#include "invoke.h"
#include "tool.h"
#include <dirent.h>
#include <linux/limits.h>
#include <unistd.h>

static char bri_path[PATH_MAX] = {0};
static char max_path[PATH_MAX] = {0};


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
