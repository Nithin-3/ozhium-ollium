/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * window.c - Window management
 */

#include "ui/window.h"
#include "ui/tool.h"
#include "ui/config.h"
#include <gtk4-layer-shell/gtk4-layer-shell.h>
#include <stdio.h>
#include <string.h>

GtkWidget *globalWindow = NULL;
static guint timeoutId = 0;

static gboolean onClose(gpointer data) {
	(void)data;
	if (globalWindow) {
		gtk_window_close(GTK_WINDOW(globalWindow));
		globalWindow = NULL;
	}
	GApplication *app = g_application_get_default();
	if (app) {
		g_application_quit(app);
	}
	return G_SOURCE_REMOVE;
}

void applyLayerShell(GtkWindow *win) {	// TODO: this function i wanna change
	gtk_layer_init_for_window(win);
	gtk_layer_set_layer(win, GTK_LAYER_SHELL_LAYER_OVERLAY);
	gtk_layer_set_exclusive_zone(win, -1);

	if (appConfig.x > -1 && appConfig.y > -1) {
		gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
		gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
		gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_TOP, appConfig.y);
		gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_LEFT, appConfig.x);
	} else {
		// Vertical alignment
		if (strcmp(appConfig.verticalAlign, "center") == 0) {
			// gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
			// gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
		} else if (strcmp(appConfig.verticalAlign, "top") == 0) {
			gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
			gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_BOTTOM, FALSE);
			gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_TOP, appConfig.margin);
		} else if (strcmp(appConfig.verticalAlign, "bottom") == 0) {
			gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_TOP, FALSE);
			gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
			gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_BOTTOM, appConfig.margin);
		}

		// Horizontal alignment
		if (strcmp(appConfig.horizontalAlign, "center") == 0) {
			// gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
			// gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
		} else if (strcmp(appConfig.horizontalAlign, "left") == 0) {
			gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
			gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_RIGHT, FALSE);
			gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_LEFT, appConfig.margin);
		} else if (strcmp(appConfig.horizontalAlign, "right") == 0) {
			gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_LEFT, FALSE);
			gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
			gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_RIGHT, appConfig.margin);
		}
	}
}

void setupCss(void) {
	char *cssPath = findConfigPath("style.css");
	if (cssPath == NULL)
		return;
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, cssPath);
	gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void resetTimer(int sec) {
	if (timeoutId != 0) {
		g_source_remove(timeoutId);
		timeoutId = 0;
	}
	timeoutId = g_timeout_add_seconds(sec, onClose, NULL);
}
