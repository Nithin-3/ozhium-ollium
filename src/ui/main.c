#include "ui/config.h"
#include "ui/window.h"
#include "ui/builder.h"
#include "ui/args.h"
#include "invoke.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

static GUI_ELEMENT elementType;

static void applyArgs(const ParsedArgs *parsed) {
	elementType = parsed->element;
	sliderArgs.min = parsed->min;
	sliderArgs.max = parsed->max;
	sliderArgs.current = parsed->current;
	sliderArgs.action = parsed->action;
	strncpy(textArgs.text, parsed->text, sizeof(textArgs.text) - 1);
	textArgs.text[sizeof(textArgs.text) - 1] = '\0';
}

static int commandLineHandler(GApplication *app, GApplicationCommandLine *cmdline, gpointer data) {
	(void)data;

	gchar **argv = g_application_command_line_get_arguments(cmdline, NULL);
	if (!argv) {
		g_application_activate(app);
		return 0;
	}

	int argc = 0;
	while (argv[argc]) argc++;

	ParsedArgs parsed;
	if (parseArgs(argc, argv, &parsed) != 0) {
		g_strfreev(argv);
		return 1;
	}
	g_strfreev(argv);

	applyArgs(&parsed);

	if (globalWindow) {
		updateContent(elementType, &sliderArgs, &textArgs);
		gtk_window_present(GTK_WINDOW(globalWindow));
	} else {
		g_application_activate(app);
	}

	if (appConfig.timeOut > 0) {
		resetTimer(appConfig.timeOut);
	}

	return 0;
}

static void onActivate(GtkApplication *app, gpointer data) {
	(void)data;

	globalWindow = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(globalWindow), "Indicator");
	gtk_window_set_resizable(GTK_WINDOW(globalWindow), FALSE);

	applyLayerShell(GTK_WINDOW(globalWindow));
	setupCss();

	GtkWidget *box;
	switch (elementType) {
		case SLIDER:
			box = buildSlider();
			break;
		case TEXT:
			box = buildText();
			break;
		default:
			box = buildText();
			break;
	}
	gtk_window_set_child(GTK_WINDOW(globalWindow), box);
	if (appConfig.timeOut > 0) {
		resetTimer(appConfig.timeOut);
	}
	gtk_window_present(GTK_WINDOW(globalWindow));
}

int main(int argc, char *argv[]) {
	loadConfig();

	GtkApplication *app = gtk_application_new("com.ozhium.ollium", G_APPLICATION_HANDLES_COMMAND_LINE);
	g_signal_connect(app, "activate", G_CALLBACK(onActivate), NULL);
	g_signal_connect(app, "command-line", G_CALLBACK(commandLineHandler), NULL);
	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}
