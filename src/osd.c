#include "invoke.h"
#include "tool.h"
#include <glib.h>
#include <gtk/gtk.h>
#include <gtk4-layer-shell/gtk4-layer-shell.h>
#include <stdio.h>

GtkWidget *globalWindow = NULL;
static guint timeoutId = 0;
static GObject *emitter;
static guint    sig_ready;

struct Config {
	char *verticalAlign, *horizontalAlign, *orientation;
	int timeOut, x, y, margin;
};

struct Config config = {
	.horizontalAlign = "center",
	.verticalAlign = "top",
	.orientation = "horizontal",
	.timeOut = 0,
	.x = 0,
	.y = 0,
	.margin = 0,
};

GKeyFile *getconf() {
	GKeyFile *keyfile = g_key_file_new();
	GError *error = NULL;
	char *path = findConfigPath("ozhium-ollium.conf");
	if (path == NULL) {
		perror("ozhium-ollium.conf file path not found\n");
		return NULL;
	}
	if (!g_key_file_load_from_file(keyfile, path, G_KEY_FILE_NONE, &error)) {
		g_print("Error: %s\n", error->message);
		g_error_free(error);
		return NULL;
	}
	printf("conf file used path: %s\n", path);
	return keyfile;
}


static void onDestroy(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
    globalWindow = NULL;
}

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

static void resetTimer(int sec) {
    if (timeoutId != 0) {
        g_source_remove(timeoutId);
        timeoutId = 0;
    }
    timeoutId = g_timeout_add_seconds(sec, onClose, NULL);
}


static void onActivate(GtkApplication *app, gpointer configData) {
	struct Config *config = (struct Config *)configData;
	emitter = g_object_new(G_TYPE_OBJECT, NULL);
	sig_ready = g_signal_new( "ready", G_TYPE_OBJECT, G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 0 );
	globalWindow = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(globalWindow), "Indicator");
	gtk_window_set_resizable(GTK_WINDOW(globalWindow), FALSE);
	g_signal_connect(globalWindow, "destroy", G_CALLBACK(onDestroy), NULL);

	gtk_layer_init_for_window(GTK_WINDOW(globalWindow));
	gtk_layer_set_layer(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_LAYER_OVERLAY);
	gtk_layer_set_exclusive_zone(GTK_WINDOW(globalWindow), -1);

	if (config->x > -1 && config->y > -1) {
		gtk_layer_set_anchor(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_TOP,TRUE);
		gtk_layer_set_anchor(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_LEFT,TRUE);
		gtk_layer_set_margin(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_TOP,config->y);
		gtk_layer_set_margin(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_LEFT,config->x);
	} else {
		if (strcmp(config->verticalAlign, "center") == 0) {
			gtk_layer_set_anchor(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_TOP,TRUE);
			gtk_layer_set_anchor(GTK_WINDOW(globalWindow),GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
		} else {
			gtk_layer_set_anchor(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_TOP,strcmp(config->verticalAlign, "top") == 0);
			gtk_layer_set_anchor(GTK_WINDOW(globalWindow),GTK_LAYER_SHELL_EDGE_BOTTOM,strcmp(config->verticalAlign, "bottom") == 0);
		}

		if (strcmp(config->horizontalAlign, "center") == 0) {
			gtk_layer_set_anchor(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_LEFT,TRUE);
			gtk_layer_set_anchor(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_RIGHT,TRUE);
		} else {
			gtk_layer_set_anchor(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_LEFT,strcmp(config->horizontalAlign, "left") == 0);
			gtk_layer_set_anchor(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_RIGHT,strcmp(config->horizontalAlign, "right") == 0);
		}
		gtk_layer_set_margin(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_TOP,config->margin);
		gtk_layer_set_margin(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_BOTTOM,config->margin);
		gtk_layer_set_margin(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_LEFT,config->margin);
		gtk_layer_set_margin(GTK_WINDOW(globalWindow), GTK_LAYER_SHELL_EDGE_RIGHT,config->margin);
	}

	g_signal_emit(emitter, sig_ready, 0);

	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, findConfigPath("style.css"));
	gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	resetTimer(config->timeOut);
}

void initSlider(sliderData *s){
	GtkOrientation orient = GTK_ORIENTATION_HORIZONTAL;
	if (strcmp(config.orientation, "vertical") == 0) {
	    orient = GTK_ORIENTATION_VERTICAL;
	}
	GtkWidget *box = gtk_box_new(orient, 10);
	gtk_window_set_child(GTK_WINDOW(globalWindow), box);
	GtkWidget *slider = gtk_scale_new_with_range(orient, (double)s->min, (double)s->max, (double)(s->max/100.0));
	gtk_scale_set_draw_value(GTK_SCALE(slider), FALSE);
	gtk_widget_set_name(slider, "slider");
	gtk_range_set_value(GTK_RANGE(slider), (double)s->current);
	// g_signal_connect(slider, "value-changed", G_CALLBACK(sliderValChang), config);
}

void initApp(GUI_ELEMENT el,void *data) {
	// GKeyFile *conf = getconf();
	// if (NULL == conf){
	// 	printf("Could not get config loading default....");
	// }
	GtkApplication *app = gtk_application_new("ozhium-ollium", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(onActivate), &config);
	g_signal_connect(emitter, "ready", G_CALLBACK(SLIDER == el? initSlider:NULL), &data);
}

