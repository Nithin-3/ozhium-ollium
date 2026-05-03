#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#include "daemon/tool.h"

char *findConfigPath(const char *filename) {
	static char path[PATH_MAX];
	const char *home = getenv("HOME");

	if (home) {
		snprintf(path, PATH_MAX, "%s/.config/ozhium-ollium/%s", home, filename);
		if (access(path, R_OK) == 0)
			return path;
	}

	snprintf(path, PATH_MAX, "/usr/share/ozhium-ollium/%s", filename);
	if (access(path, R_OK) == 0)
		return path;

	return NULL;
}


char *strReplace(const char *src, const char *find, const char *replace) {
    char *result;
    const char *pos = strstr(src, find);
    if (!pos)
        return strdup(src); // not found, return copy as-is

    size_t find_len    = strlen(find);
    size_t replace_len = strlen(replace);
    size_t prefix_len  = pos - src;
    size_t suffix_len  = strlen(pos + find_len);

    result = malloc(prefix_len + replace_len + suffix_len + 1);
    if (!result) return NULL;

    memcpy(result, src, prefix_len);
    memcpy(result + prefix_len, replace, replace_len);
    memcpy(result + prefix_len + replace_len, pos + find_len, suffix_len + 1); // +1 for '\0'

    return result;
}

