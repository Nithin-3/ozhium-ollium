/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * config.h - UI configuration
 */
#pragma once

#include <stdbool.h>

/**
 * @brief Application configuration structure
 */
typedef struct {
	char *verticalAlign, *horizontalAlign, *orientation;
	int timeOut, x, y, margin;
} AppConfig;

/**
 * @brief Icon configuration for different actions
 */
typedef struct {
	char *backlight, *volume, *volumeMute, *mic, *micMute;
	char *batLow, *batFull, *batCharge, *batDischarge, *batIdel, *batCrit, *wifi, *ethernet, *bluetooth;
} Icons;

/**
 * @brief Slider widget configuration
 */
typedef struct {
	bool invert_dir;
	char *label1, *label2;
} SliderConfig;

/**
 * @brief Text/label widget configuration
 */
typedef struct {
	char *label;
} TextConfig;

/** @brief Global application configuration */
extern AppConfig appConfig;
/** @brief Global icon configuration */
extern Icons icons;
/** @brief Global slider configuration */
extern SliderConfig sliderConfig;
/** @brief Global text configuration */
extern TextConfig textConfig;

/**
 * @brief Load configuration from config file
 */
void loadConfig(void);
