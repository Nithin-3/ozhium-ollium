/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * window.h - Window management
 */
#pragma once

#include <gtk/gtk.h>

/** @brief Global window widget reference */
extern GtkWidget *globalWindow;

/**
 * @brief Apply layer-shell settings to a GTK window
 * @param win The GTK window to configure
 */
void applyLayerShell(GtkWindow *win);

/**
 * @brief Load and apply CSS styling from configuration
 */
void setupCss(void);

/**
 * @brief Reset the auto-close timer
 * @param sec Number of seconds before window auto-closes
 */
void resetTimer(int sec);