#ifndef UI_CONFIG_H
#define UI_CONFIG_H

#include <glib.h>

typedef struct {
	char *verticalAlign, *horizontalAlign, *orientation;
	int timeOut, x, y, margin;
} AppConfig;

typedef struct {
	char *backlight, *volume,*volumeMute;
} Icons;

extern AppConfig appConfig;
extern Icons icons;

void loadConfig(void);

#endif
