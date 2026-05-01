#ifndef UI_CONFIG_H
#define UI_CONFIG_H

#include <glib.h>

typedef struct {
	char *verticalAlign, *horizontalAlign, *orientation;
	int timeOut, x, y, margin;
} AppConfig;

extern AppConfig appConfig;

void loadConfig(void);

#endif
