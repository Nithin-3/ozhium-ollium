/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * args.h - Command-line argument parsing
 */
#pragma once

#include "shared/common.h"

/**
 * @brief Parsed command-line arguments structure
 */
typedef struct {
	GUI_ELEMENT element;
	float min, max, current;
	ACTION action;
	char text[256];
} ParsedArgs;

/**
 * @brief Convert string to GUI element enum
 * @param s String representation ("0", "slider", "text")
 * @return Corresponding GUI_ELEMENT value
 */
GUI_ELEMENT argToElement(const char *s);

/**
 * @brief Convert string to slider action enum
 * @param s String representation ("s0", "backlight", "audio", etc.)
 * @return Corresponding SLIDER_ACTION value
 */
ACTION argToSliderAction(const char *s);

/**
 * @brief Parse command-line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @param args Pointer to ParsedArgs to store results
 * @return 0 on success, non-zero on failure
 */
int parseArgs(int argc, char *argv[], ParsedArgs *args);
