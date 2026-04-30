#include <gtk/gtk.h>
#include <gtk4-layer-shell/gtk4-layer-shell.h>
#include <glib.h>
#include <stdio.h>

// find config path ==================

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_PATH 512

static inline char* findConfigPath(const char* filename) {
    static char path[MAX_PATH];
    const char* home = getenv("HOME");

    if (home) {
        snprintf(path, MAX_PATH, "%s/.config/echo-meter/%s", home, filename);
        if (access(path, R_OK) == 0)
            return path;
    }

    snprintf(path, MAX_PATH, "/usr/share/echo-meter/%s", filename);
    if (access(path, R_OK) == 0)
        return path;

    return NULL;
}

// ===========================


GKeyFile* getconf(){
	GKeyFile *keyfile = g_key_file_new();
	GError *error = NULL;
	char* path = findConfigPath("ozhium-ollium.conf");
	if(path == NULL){
		perror("ozhium-ollium.conf file path not found\n");
		return NULL;
	}
	if (!g_key_file_load_from_file(keyfile, path, G_KEY_FILE_NONE, &error)) {
		g_print("Error: %s\n", error->message);
		g_error_free(error);
		return NULL;
	}
	printf("conf file used path: %s\n",path);
	return keyfile;
}

