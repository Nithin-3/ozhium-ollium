#include "ui/builder.h"
#include "ui/config.h"
#include "invoke.h"
#include <gtk/gtk.h>
#include <string.h>

sliderData sliderArgs;
textData textArgs;

GtkWidget *buildSlider(void) {
	GtkOrientation orient = GTK_ORIENTATION_HORIZONTAL;
	if (strcmp(appConfig.orientation, "vertical") == 0)
		orient = GTK_ORIENTATION_VERTICAL;

	GtkWidget *slider = gtk_scale_new_with_range(orient, (double)sliderArgs.min, (double)sliderArgs.max, (double)(sliderArgs.max / 100.0));
	gtk_scale_set_draw_value(GTK_SCALE(slider), FALSE);
	gtk_widget_set_name(slider, "slider");
	gtk_range_set_value(GTK_RANGE(slider), (double)sliderArgs.current);

	GtkWidget *labelBefore = gtk_label_new("bef");
	GtkWidget *labelAfter = gtk_label_new("aft");
	gtk_widget_set_name(labelBefore, "slider-label-before");
	gtk_widget_set_name(labelAfter, "slider-label-after");

	GtkWidget *box = gtk_box_new(orient, 6);
	gtk_box_append(GTK_BOX(box), labelBefore);
	gtk_widget_set_hexpand(slider, TRUE);
	gtk_box_append(GTK_BOX(box), slider);
	gtk_box_append(GTK_BOX(box), labelAfter);

	return box;
}

GtkWidget *buildText(void) {
	GtkWidget *label = gtk_label_new(textArgs.text);
	gtk_widget_set_name(label, "text");
	return label;
}
