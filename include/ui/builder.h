/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * builder.h - GTK widget builder
 */
#pragma once

#include "shared/common.h"
#include <gtk/gtk.h>

GtkWidget *initBox(void);
/**
 * @brief Build a slider widget with given data
 * @param s Pointer to sliderData containing slider parameters
 * @return GTK widget containing the slider
 */
void buildSlider(const sliderData *s);

/**
 * @brief Build a text/label widget with given data
 * @param t Pointer to textData containing text parameters
 * @return GTK widget containing the text label
 */
void buildText(const textData *t);

/**
 * @brief Update existing widget content based on element type
 * @param el The GUI element type (SLIDER or TEXT)
 * @param s Pointer to slider data (used if el is SLIDER)
 * @param t Pointer to text data (used if el is TEXT)
 */
void updateContent(GUI_ELEMENT el, const sliderData *s, const textData *t);
