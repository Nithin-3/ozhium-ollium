/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * builder.c - Widget building
 */

#include "ui/builder.h"
#include "gtk/gtk.h"
#include "gtk/gtkshortcut.h"
#include "shared/common.h"
#include "ui/config.h"
#include "ui/window.h"
#include <stdio.h>
#include <string.h>

static GtkWidget *box = NULL;

const char sldr[] = "slider";
const char sldrLB[] = "slider-label-before";
const char sldrLA[] = "slider-label-after";
const char txt[] = "text";

char *getIconForAction(ACTION action);

/**
 * Build the substituted label strings for a slider.
 * Caller is responsible for free()-ing *out_l1 and *out_l2.
 */
void buildLabels(const sliderData *s, char **out_l1, char **out_l2);

GtkWidget *initBox(void) {
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    return box;
}


static void clearBox(void) {
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(box)) != NULL)
        gtk_box_remove(GTK_BOX(box), child);
}

void applySlider(const sliderData *s) {
	char *l1, *l2;
	buildLabels(s, &l1, &l2);

	for (GtkWidget *child = gtk_widget_get_first_child(box); child != NULL; child = gtk_widget_get_next_sibling(child)) {

		const char *name = gtk_widget_get_name(child);
		if (strcmp(name, sldr)   == 0) gtk_range_set_value(GTK_RANGE(child), (double)s->current);
		else if (strcmp(name, sldrLB) == 0) gtk_label_set_text(GTK_LABEL(child), l1);
		else if (strcmp(name, sldrLA) == 0) gtk_label_set_text(GTK_LABEL(child), l2);
	}

	free(l1);
	free(l2);
}

void applyText(const textData *t) {
    char *tmp  = strReplace(textConfig.label, "#ico#", getIconForAction(t->action));
    char *text = strReplace(tmp, "#val#", t->text);
    free(tmp);

    for (GtkWidget *child = gtk_widget_get_first_child(box); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        if (strcmp(gtk_widget_get_name(child), txt) == 0) gtk_label_set_text(GTK_LABEL(child), text);
    }

    free(text);
}

void buildSlider(const sliderData *s) {
	clearBox();

	GtkOrientation orient = strcmp(appConfig.orientation, "vertical") == 0 ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL;

	gtk_orientable_set_orientation(GTK_ORIENTABLE(box), orient);
	gtk_box_set_spacing(GTK_BOX(box), 6);
	gtk_widget_set_name(box, "slider-box");

	GtkWidget *sliderWidget = gtk_scale_new_with_range(orient, (double)s->min, (double)s->max, s->max / 100.0);
	gtk_scale_set_draw_value(GTK_SCALE(sliderWidget), FALSE);
	gtk_widget_set_name(sliderWidget, sldr);
	gtk_widget_set_hexpand(sliderWidget, TRUE);
	gtk_widget_set_vexpand(sliderWidget, TRUE);
	gtk_widget_set_sensitive(sliderWidget, FALSE);
	gtk_range_set_value(GTK_RANGE(sliderWidget), (double)s->current);

	if (orient == GTK_ORIENTATION_HORIZONTAL) {
		gtk_widget_set_size_request(sliderWidget, 100, -1);
		gtk_range_set_inverted(GTK_RANGE(sliderWidget), sliderConfig.invert_dir);
	} else {
		gtk_widget_set_size_request(sliderWidget, -1, 100);
		gtk_range_set_inverted(GTK_RANGE(sliderWidget), !sliderConfig.invert_dir);
	}

	char *l1, *l2;
	buildLabels(s, &l1, &l2);

	if (l1 && l1[0]) {
		GtkWidget *lb = gtk_label_new(l1);
		gtk_widget_set_name(lb, sldrLB);
		gtk_box_append(GTK_BOX(box), lb);
	}
	gtk_box_append(GTK_BOX(box), sliderWidget);
	if (l2 && l2[0]) {
		GtkWidget *la = gtk_label_new(l2);
		gtk_widget_set_name(la, sldrLA);
		gtk_box_append(GTK_BOX(box), la);
	}

	free(l1);
	free(l2);
}

void buildText(const textData *t) {
	clearBox();

	gtk_orientable_set_orientation(GTK_ORIENTABLE(box), GTK_ORIENTATION_HORIZONTAL);
	gtk_box_set_spacing(GTK_BOX(box), 10);
	gtk_widget_set_name(box, "text-box");

	GtkWidget *textWidget = gtk_label_new(NULL);
	gtk_widget_set_name(textWidget, txt);
	gtk_label_set_xalign(GTK_LABEL(textWidget), 0.5f);
	gtk_label_set_yalign(GTK_LABEL(textWidget), 0.5f);
	gtk_widget_set_hexpand(textWidget, TRUE);
	gtk_widget_set_vexpand(textWidget, TRUE);
	gtk_box_append(GTK_BOX(box), textWidget); // add first, then apply
	applyText(t);
}

void updateContent(GUI_ELEMENT el, const sliderData *s, const textData *t) {
	printf("[updateContent] element=%d\n", el);

	const char *boxName = gtk_widget_get_name(box);

	switch (el) {
		case SLIDER:
			if (strcmp(boxName, "slider-box") == 0) applySlider(s);
			else buildSlider(s);
			break;
		case TEXT:
			if (strcmp(boxName, "text-box") == 0) applyText(t);
			else buildText(t);
			break;
		default:
			printf("[updateContent] WARNING: unknown element %d\n", el);
			break;
	}

	gtk_window_present(GTK_WINDOW(globalWindow));
}

char *getIconForAction(ACTION action) {
	switch (action) {
		case BACKLIGHT:
			return icons.backlight;
		case AUDIO:
			return icons.volume;
		case AUDIO_MUTE:
			return icons.volumeMute;
		case MIC:
			return icons.mic;
		case MIC_MUTE:
			return icons.micMute;
		case BAT_LOW:
			return icons.batLow;
		case BAT_FULL:
			return icons.batFull;
		case BAT_CHARGE:
			return icons.batCharge;
		case BAT_DISCHARGE:
			return icons.batDischarge;
		case BAT_IDEL:
			return icons.batIdel;
		case WIFI:
			return icons.wifi;
		case ETHERNET:
			return icons.ethernet;
		case BLUETOOTH:
			return icons.bluetooth;
		default:
			return " ";
	}
}


void buildLabels(const sliderData *s, char **out_l1, char **out_l2) {
	gchar *textBuf = g_strdup_printf( "%d", s->current > 1 ? (int)s->current : (int)(s->current * 100));
	const char *ico = getIconForAction(s->action);

	char *tmp;

	tmp = strReplace(sliderConfig.label1, "#ico#", ico);
	*out_l1 = strReplace(tmp, "#val#", textBuf);
	free(tmp);

	tmp = strReplace(sliderConfig.label2, "#ico#", ico);
	*out_l2 = strReplace(tmp, "#val#", textBuf);
	free(tmp);

	g_free(textBuf);
}

