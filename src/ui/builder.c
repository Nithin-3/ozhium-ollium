#include "ui/builder.h"
#include "ui/config.h"
#include "invoke.h"
#include <string.h>
#include <stdio.h>

static GtkWidget *sliderWidget = NULL;
static GtkWidget *labelBefore = NULL;
static GtkWidget *labelAfter = NULL;
static GtkWidget *textWidget = NULL;


static char *getIconForAction(SLIDER_ACTION action) {
	switch (action) {
		case BACKLIGHT:
			return icons.backlight;
		case AUDIO:
			return icons.volume;
			break;
		case AUDIO_MUTE:
			return icons.volumeMute;
		default:
			return icons.backlight;
	}
}

void applySlider(sliderData *s){
	printf("[APPLY_SLIDER] Called with: current=%.2f, action=%d\n", s->current, s->action);
	gtk_range_set_value(GTK_RANGE(sliderWidget), (double)s->current);
	gtk_label_set_text(GTK_LABEL(labelBefore), getIconForAction(s->action));
	gchar *textBuf = g_strdup_printf("%d%%", (int)(s->current * 100));
	gtk_label_set_text(GTK_LABEL(labelAfter), textBuf);
	printf("[APPLY_SLIDER] Display: %s\n", textBuf);
	g_free(textBuf);
}

void applyText(textData *t){
	printf("[APPLY_TEXT] Called with: text='%s'\n", t->text);
	if (textWidget) {
		gtk_label_set_text(GTK_LABEL(textWidget), t->text);
		printf("[APPLY_TEXT] Text successfully applied to widget\n");
	} else {
		printf("[APPLY_TEXT] WARNING: textWidget is NULL\n");
	}
}

GtkWidget *buildSlider(const sliderData *s) {
	printf("[buildSlider] Called with: min=%.2f, max=%.2f, current=%.2f, action=%d\n", s->min, s->max, s->current, s->action);
	
	GtkOrientation orient = GTK_ORIENTATION_HORIZONTAL;
	if (strcmp(appConfig.orientation, "vertical") == 0)
		orient = GTK_ORIENTATION_VERTICAL;

	sliderWidget = gtk_scale_new_with_range(orient, (double)s->min, (double)s->max, (double)(s->max / 100.0));
	gtk_scale_set_draw_value(GTK_SCALE(sliderWidget), FALSE);
	gtk_widget_set_name(sliderWidget, "slider");
	gtk_widget_set_hexpand(sliderWidget, TRUE);
	gtk_widget_set_vexpand(sliderWidget, TRUE);
	gtk_widget_set_sensitive(sliderWidget, FALSE);
	
	// Set minimum size for slider (horizontal: 200px width, vertical: 200px height)
	if (orient == GTK_ORIENTATION_HORIZONTAL) {
		gtk_widget_set_size_request(sliderWidget, 200, -1);
	} else {
		gtk_widget_set_size_request(sliderWidget, -1, 200);
	}

	labelBefore = gtk_label_new("");
	labelAfter = gtk_label_new("");
	gtk_widget_set_name(labelBefore, "slider-label-before");
	gtk_widget_set_name(labelAfter, "slider-label-after");

	// Apply initial values
	sliderData *mutableSlider = (sliderData *)s;
	applySlider(mutableSlider);

	GtkWidget *box = gtk_box_new(orient, 6);
	
	gtk_widget_set_name(box, "box");
	gtk_widget_set_hexpand(box, TRUE);
	gtk_widget_set_vexpand(box, TRUE);
	gtk_box_append(GTK_BOX(box), labelBefore);
	gtk_box_append(GTK_BOX(box), sliderWidget);
	gtk_box_append(GTK_BOX(box), labelAfter);

	textWidget = NULL;

	return box;
}

GtkWidget *buildText(const textData *t) {
	printf("[buildText] Called with: text='%s'\n", t->text);
	
	textWidget = gtk_label_new(t->text);
	gtk_widget_set_name(textWidget, "text");
	gtk_label_set_xalign(GTK_LABEL(textWidget), 0.5);
	gtk_label_set_yalign(GTK_LABEL(textWidget), 0.5);
	gtk_widget_set_hexpand(textWidget, TRUE);
	gtk_widget_set_vexpand(textWidget, TRUE);

	sliderWidget = NULL;

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_widget_set_name(box, "box");
	gtk_box_append(GTK_BOX(box), textWidget);

	return box;
}

void updateContent(GUI_ELEMENT el, const sliderData *s, const textData *t) {
	printf("[updateContent] Called with element=%d\n", el);
	
	switch (el) {
		case SLIDER:
			if (sliderWidget) {
				printf("[updateContent] SLIDER: min=%.2f, max=%.2f, current=%.2f, action=%d\n", 
					s->min, s->max, s->current, s->action);
				
				sliderData *mutableSlider = (sliderData *)s;
				applySlider(mutableSlider);
			}
			break;
		case TEXT:
			if (textWidget) {
				printf("[updateContent] TEXT: text='%s'\n", t->text);
				
				textData *mutableText = (textData *)t;
				applyText(mutableText);
			}
			break;
	}
}
