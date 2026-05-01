#include "ui/config.h"
#include "tool.h"

AppConfig appConfig = {
	.horizontalAlign = "center",
	.verticalAlign = "top",
	.orientation = "horizontal",
	.timeOut = 3,
	.x = 0,
	.y = 0,
	.margin = 0,
};

Icons icons = {
	.backlight = "",
	.volume = "",
	.volumeMute = "󰖁",
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

	if (g_key_file_has_key(keyfile, "osd", "vertical-align", NULL))
		appConfig.verticalAlign = g_key_file_get_string(keyfile, "osd", "vertical-align", NULL);
	if (g_key_file_has_key(keyfile, "osd", "horizontal-align", NULL))
		appConfig.horizontalAlign = g_key_file_get_string(keyfile, "osd", "horizontal-align", NULL);
	if (g_key_file_has_key(keyfile, "osd", "orientation", NULL))
		appConfig.orientation = g_key_file_get_string(keyfile, "osd", "orientation", NULL);
	if (g_key_file_has_key(keyfile, "osd", "timeout", NULL))
		appConfig.timeOut = g_key_file_get_integer(keyfile, "osd", "timeout", NULL);
	if (g_key_file_has_key(keyfile, "osd", "x", NULL))
		appConfig.x = g_key_file_get_integer(keyfile, "osd", "x", NULL);
	if (g_key_file_has_key(keyfile, "osd", "y", NULL))
		appConfig.y = g_key_file_get_integer(keyfile, "osd", "y", NULL);
	if (g_key_file_has_key(keyfile, "osd", "margin", NULL))
		appConfig.margin = g_key_file_get_integer(keyfile, "osd", "margin", NULL);

	if (g_key_file_has_key(keyfile, "icons", "backlight", NULL))
		icons.backlight = g_key_file_get_string(keyfile, "icons", "backlight", NULL);
	if (g_key_file_has_key(keyfile, "icons", "volume", NULL))
		icons.volume = g_key_file_get_string(keyfile, "icons", "volume", NULL);
	if (g_key_file_has_key(keyfile, "icons", "volume-mute", NULL))
		icons.volumeMute = g_key_file_get_string(keyfile, "icons", "volume-mute", NULL);

	g_key_file_unref(keyfile);
}
