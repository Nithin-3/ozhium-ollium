#include "ui/config.h"
#include "ui/window.h"
#include "ui/builder.h"
#include "ui/args.h"
#include "invoke.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

static GUI_ELEMENT elementType;

static void onActivate(GtkApplication *app, gpointer data) {
	(void)data;
	GtkWidget *win = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(win), "Indicator");
	gtk_window_set_resizable(GTK_WINDOW(win), FALSE);

	applyLayerShell(GTK_WINDOW(win));
	setupCss();

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_widget_set_size_request(box, 280, 60);
	gtk_widget_set_hexpand(box, FALSE);
	gtk_widget_set_vexpand(box, FALSE);

	switch (elementType) {
		case SLIDER: {
			GtkWidget *slider = buildSlider();
			gtk_widget_set_hexpand(slider, TRUE);
			gtk_widget_set_vexpand(slider, FALSE);
			gtk_box_append(GTK_BOX(box), slider);
			break;
		}
		case TEXT: {
			GtkWidget *label = buildText();
			gtk_widget_set_hexpand(label, TRUE);
			gtk_widget_set_vexpand(label, TRUE);
			gtk_label_set_xalign(GTK_LABEL(label), 0.5);
			gtk_label_set_yalign(GTK_LABEL(label), 0.5);
			gtk_box_append(GTK_BOX(box), label);
			break;
		}
	}

	gtk_window_set_child(GTK_WINDOW(win), box);

	if (appConfig.timeOut > 0) {
		resetTimer(appConfig.timeOut);
	}

	gtk_window_present(GTK_WINDOW(win));
}

int main(int argc, char *argv[]) {
	ParsedArgs parsed;
	if (parseArgs(argc, argv, &parsed) != 0) return 1;

	elementType = parsed.element;
	sliderArgs.min = parsed.min;
	sliderArgs.max = parsed.max;
	sliderArgs.current = parsed.current;
	sliderArgs.action = parsed.action;
	strncpy(textArgs.text, parsed.text, sizeof(textArgs.text) - 1);
	textArgs.text[sizeof(textArgs.text) - 1] = '\0';

	loadConfig();

	GtkApplication *app = gtk_application_new("com.ozhium.ollium", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(onActivate), NULL);
	int status = g_application_run(G_APPLICATION(app), 0, NULL);
	g_object_unref(app);

	return status;
}
