/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * invoke.h - UI invocation
 */
#pragma once

#include "shared/common.h"

/**
 * @brief Execute the UI process with specified element and data
 * @param element The GUI element type (SLIDER or TEXT)
 * @param data Pointer to sliderData or textData depending on element type
 */
void execUI(const ACTION action, void *data);
