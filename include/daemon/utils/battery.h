/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * battery.h - Battery monitoring
 */
#pragma once

#include "shared/common.h"
#include <stddef.h>

/**
 * @brief Get current battery status
 * @param t Pointer to textData to store the battery info
 * @return 0 on success, non-zero on failure
 */
int getBattery(textData *t, char *path);

int findBatteryPath(char *buf, size_t len);

