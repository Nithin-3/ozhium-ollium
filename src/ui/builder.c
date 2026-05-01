#include "ui/builder.h"
#include "ui/config.h"
#include "invoke.h"
#include <string.h>

sliderData sliderArgs;
textData textArgs;

static GtkWidget *sliderWidget = NULL;
static GtkWidget *textWidget = NULL;

GtkWidget *buildSlider(void) {
	GtkOrientation orient = GTK_ORIENTATION_HORIZONTAL;
	if (strcmp(appConfig.orientation, "vertical") == 0)
		orient = GTK_ORIENTATION_VERTICAL;

	sliderWidget = gtk_scale_new_with_range(orient, (double)sliderArgs.min, (double)sliderArgs.max, (double)(sliderArgs.max / 100.0));
	gtk_scale_set_draw_value(GTK_SCALE(sliderWidget), FALSE);
	gtk_widget_set_name(sliderWidget, "slider");
	gtk_range_set_value(GTK_RANGE(sliderWidget), (double)sliderArgs.current);
	gtk_widget_set_hexpand(sliderWidget, TRUE);

	GtkWidget *labelBefore = gtk_label_new("bef");
	GtkWidget *labelAfter = gtk_label_new("aft");
	gtk_widget_set_name(labelBefore, "slider-label-before");
	gtk_widget_set_name(labelAfter, "slider-label-after");

	GtkWidget *box = gtk_box_new(orient, 6);
	gtk_box_append(GTK_BOX(box), labelBefore);
	gtk_box_append(GTK_BOX(box), sliderWidget);
	gtk_box_append(GTK_BOX(box), labelAfter);

	textWidget = NULL;

	return box;
}

GtkWidget *buildText(void) {
	textWidget = gtk_label_new(textArgs.text);
	gtk_widget_set_name(textWidget, "text");
	gtk_label_set_xalign(GTK_LABEL(textWidget), 0.5);
	gtk_label_set_yalign(GTK_LABEL(textWidget), 0.5);
	gtk_widget_set_hexpand(textWidget, TRUE);
	gtk_widget_set_vexpand(textWidget, TRUE);

	sliderWidget = NULL;

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_append(GTK_BOX(box), textWidget);

	return box;
}

// TODO: create function for apply value for build*

void updateContent(GUI_ELEMENT el, const sliderData *s, const textData *t) {
	switch (el) {
		case SLIDER:
			if (sliderWidget) {
				gtk_range_set_value(GTK_RANGE(sliderWidget), (double)s->current);
				// TODO: update slider data
				// call sliderApply
			}
			break;
		case TEXT:
			if (textWidget) {
				gtk_label_set_text(GTK_LABEL(textWidget), t->text);
				// TODO: update text data
				// call textApply
			}
			break;
	}
}
