#pragma once

#include "shared/common.h"
#include <stddef.h>

/**
 * @brief Get current battery status
 * @param t Pointer to textData to store the battery info
 * @return 0 on success, non-zero on failure
 */
int getBattery(textData *t);

/**
 * @brief Find battery device paths in sysfs
 * @param cap Buffer to store battery capacity file path
 * @param sta Buffer to store battery status file path
 * @param sz Size of the path buffers
 * @return 0 on success, 1 if no battery found
 */
int findBatteryPaths(char *cap, char *sta, size_t sz);