#pragma once

#include "shared/common.h"
#include <linux/limits.h>
#include <stddef.h>

/** @brief Global backlight brightness file path */
extern char bri_path[PATH_MAX];
/** @brief Global backlight max brightness file path */
extern char max_path[PATH_MAX];

/**
 * @brief Get current backlight brightness
 * @param s Pointer to sliderData to store the result
 * @return 0 on success, non-zero on failure
 */
int getBacklight(sliderData *s);

/**
 * @brief Find backlight device paths in sysfs
 * @param bri_path Buffer to store brightness file path
 * @param max_path Buffer to store max brightness file path
 * @param size Size of the path buffers
 * @return 0 on success, 1 if no backlight device found
 */
int findBacklightPaths(char *bri_path, char *max_path, size_t size);