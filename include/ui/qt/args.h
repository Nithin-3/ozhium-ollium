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

typedef struct {
	GUI_ELEMENT element;
	float min, max, current;
	ACTION action;
	char text[256];
} ParsedArgs;

GUI_ELEMENT argToElement(const char *s);
ACTION argToAction(const char *s);
int parseArgs(int argc, char *argv[], ParsedArgs *args);
