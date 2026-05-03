#pragma once

#include "shared/common.h"
#include <linux/limits.h>
#include <stddef.h>


extern char bri_path[PATH_MAX];
extern char max_path[PATH_MAX];

int getBacklight(sliderData *s);
int findBacklightPaths(char *bri_path, char *max_path, size_t size);
