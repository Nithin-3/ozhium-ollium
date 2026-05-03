#include "ui/config.h"
#include "glib.h"
#include "glibconfig.h"
#include "daemon/tool.h"

AppConfig appConfig = {
	.horizontalAlign = "center",
	.verticalAlign = "top",
	.orientation = "horizontal",
	.timeOut = 3,
	.x = -1,
	.y = -1,
	.margin = 0,
};

Icons icons = {
	.backlight = "",
	.volume = "",
	.volumeMute = "󰖁",
	.mic = "󰍬",
	.micMute = "",
	.batCharge = "󰂄",
	.batDischarge="󰁾",
	.batFull="",
	.batLow="󱊡",
	.batIdel="󰁹"
};

SliderConfig sliderConfig = {
	.invert_dir = FALSE,
	.label1 = "#ico#",
	.label2 = "#val#",
};

TextConfig textConfig = {
	.label = "#ico# #val#",
};

void loadConfig(void) {
	GKeyFile *keyfile = g_key_file_new();
	GError *error = NULL;
	char *path = findConfigPath("ozhium-ollium.conf");
	if (path == NULL) return;
	if (!g_key_file_load_from_file(keyfile, path, G_KEY_FILE_NONE, &error)) {
		g_error_free(error);
		g_key_file_unref(keyfile);
		return;
	}

	// set appConfig ==========================
	if (g_key_file_has_key(keyfile, "osd", "vertical-align", NULL)) {
		gchar *val = g_key_file_get_string(keyfile, "osd", "vertical-align", &error);
		if (val && (strcmp(val, "top") == 0 || strcmp(val, "center") == 0 || strcmp(val, "bottom") == 0))
			appConfig.verticalAlign = val;
		else
			g_free(val);
	}
	if (g_key_file_has_key(keyfile, "osd", "horizontal-align", NULL)) {
		gchar *val = g_key_file_get_string(keyfile, "osd", "horizontal-align", &error);
		if (val && (strcmp(val, "left") == 0 || strcmp(val, "right") == 0 || strcmp(val, "center") == 0))
			appConfig.horizontalAlign = val;
		else
			g_free(val);
	}
	if (g_key_file_has_key(keyfile, "osd", "orientation", NULL)) {
		gchar *val = g_key_file_get_string(keyfile, "osd", "orientation", &error);
		if (val && (strcmp(val, "vertical") == 0 || strcmp(val, "horizontal") == 0))
			appConfig.orientation = val;
		else
			g_free(val);
	}
	if (g_key_file_has_key(keyfile, "osd", "timeout", NULL)) {
		gint val = g_key_file_get_integer(keyfile, "osd", "timeout", &error);
		if (!error && val > 0)
			appConfig.timeOut = val;
	}
	if (g_key_file_has_key(keyfile, "osd", "x", NULL)) {
		gint val = g_key_file_get_integer(keyfile, "osd", "x", &error);
		if (!error && val >= 0)
			appConfig.x = val;
	}
	if (g_key_file_has_key(keyfile, "osd", "y", NULL)) {
		gint val = g_key_file_get_integer(keyfile, "osd", "y", &error);
		if (!error && val >= 0)
			appConfig.y = val;
	}
	if (g_key_file_has_key(keyfile, "osd", "margin", NULL)) {
		gint val = g_key_file_get_integer(keyfile, "osd", "margin", &error);
		if (!error && val >= 0)
			appConfig.margin = val;
	}
	// =======================


	// set sliderConfig =============================
	if (g_key_file_has_key(keyfile, "slider", "invert_dir", NULL)) {
		gboolean val = g_key_file_get_boolean(keyfile, "slider", "invert_dir", &error);
		if (!error)
			sliderConfig.invert_dir = val;
	}
	if (g_key_file_has_key(keyfile, "slider", "label1", NULL))
		sliderConfig.label1 = g_key_file_get_string(keyfile, "slider", "label1", &error);
	if (g_key_file_has_key(keyfile, "slider", "label2", NULL))
		sliderConfig.label2 = g_key_file_get_string(keyfile, "slider", "label2", &error);
	// ===============
	// set textConfig ====================
	
	if (g_key_file_has_key(keyfile, "text", "label", NULL))
		textConfig.label = g_key_file_get_string(keyfile, "text", "label", &error);

	//=============================

	// set icons ==========================
	if (g_key_file_has_key(keyfile, "icons", "backlight", NULL))
		icons.backlight = g_key_file_get_string(keyfile, "icons", "backlight", &error);
	if (g_key_file_has_key(keyfile, "icons", "volume", NULL))
		icons.volume = g_key_file_get_string(keyfile, "icons", "volume", &error);
	if (g_key_file_has_key(keyfile, "icons", "volume-mute", NULL))
		icons.volumeMute = g_key_file_get_string(keyfile, "icons", "volume-mute", &error);
	if (g_key_file_has_key(keyfile, "icons", "mic", NULL))
		icons.mic = g_key_file_get_string(keyfile, "icons", "mic", &error);
	if (g_key_file_has_key(keyfile, "icons", "mic-mute", NULL))
		icons.micMute = g_key_file_get_string(keyfile, "icons", "mic-mute", &error);
	if (g_key_file_has_key(keyfile, "icons", "bat-low", NULL))
		icons.batLow = g_key_file_get_string(keyfile, "icons", "bat-low", &error);
	if (g_key_file_has_key(keyfile, "icons", "bat-full", NULL))
		icons.batFull = g_key_file_get_string(keyfile, "icons", "bat-full", &error);
	if (g_key_file_has_key(keyfile, "icons", "bat-charge", NULL))
		icons.batCharge = g_key_file_get_string(keyfile, "icons", "bat-charge", &error);
	if (g_key_file_has_key(keyfile, "icons", "bat-discharge", NULL))
		icons.batDischarge = g_key_file_get_string(keyfile, "icons", "bat-discharge", &error);
	if (g_key_file_has_key(keyfile, "icons", "bat-idel", NULL))
		icons.batIdel = g_key_file_get_string(keyfile, "icons", "bat-idel", &error);
	// ====================

	g_key_file_unref(keyfile);
}
