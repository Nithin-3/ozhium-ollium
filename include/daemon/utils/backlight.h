/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * backlight.h - Backlight control
 */
#pragma once

#include "shared/common.h"
#include <linux/limits.h>
#include <stddef.h>

/**
 * @brief Get current backlight brightness
 * @param s Pointer to sliderData to store the result
 * @return 0 on success, non-zero on failure
 */
int getBacklight(sliderData *s, char *path);
