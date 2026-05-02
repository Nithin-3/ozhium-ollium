#include "ui/config.h"
#include "ui/window.h"
#include "ui/builder.h"
#include "ui/args.h"
#include "invoke.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

static GUI_ELEMENT elementType;
static ParsedArgs currentArgs;

static void onActivate(GtkApplication *app, gpointer data) {
	(void)data;

	printf("[onActivate] Called with elementType=%d\n", elementType);

	globalWindow = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(globalWindow), "Indicator");
	gtk_window_set_resizable(GTK_WINDOW(globalWindow), FALSE);

	applyLayerShell(GTK_WINDOW(globalWindow));
	setupCss();

	GtkWidget *box;
	switch (elementType) {
		case SLIDER: {
			printf("[onActivate] Building slider with passed data\n");
			sliderData s = {currentArgs.min, currentArgs.max, currentArgs.current, currentArgs.action.slider};
			box = buildSlider(&s);
			break;
		}
		case TEXT: {
			printf("[onActivate] Building text with passed data\n");
			textData t;
			strncpy(t.text, currentArgs.text, sizeof(t.text) - 1);
			t.text[sizeof(t.text) - 1] = '\0';
			t.action = currentArgs.action.text;
			box = buildText(&t);
			break;
		}
		default: {
			textData t = {"", BAT_LOW};
			box = buildText(&t);
			break;
		}
	}
	gtk_window_set_child(GTK_WINDOW(globalWindow), box);
	if (appConfig.timeOut > 0) {
		resetTimer(appConfig.timeOut);
	}
	gtk_window_present(GTK_WINDOW(globalWindow));
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
	
	if (parseArgs(argc, argv, &currentArgs) != 0) {
		g_strfreev(argv);
		return 1;
	}
	g_strfreev(argv);

	elementType = currentArgs.element;
	
	if (globalWindow) {
		sliderData s = {currentArgs.min, currentArgs.max, currentArgs.current, currentArgs.action.slider};
		textData t;
		strncpy(t.text, currentArgs.text, sizeof(t.text) - 1);
		t.text[sizeof(t.text) - 1] = '\0';
		t.action = currentArgs.action.text;
		
		printf("[commandLineHandler] Calling updateContent with proper structs\n");
		updateContent(elementType, &s, &t);
		gtk_window_present(GTK_WINDOW(globalWindow));
	} else {
		g_application_activate(app);
	}

	if (appConfig.timeOut > 0) {
		resetTimer(appConfig.timeOut);
	}

	return 0;
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
