#ifndef UI_CONFIG_H
#define UI_CONFIG_H

#include <glib.h>
#include <stdbool.h>

typedef struct {
	char *verticalAlign, *horizontalAlign, *orientation;
	int timeOut, x, y, margin;
} AppConfig;

typedef struct {
	char *backlight, *volume, *volumeMute, *mic, *micMute;
} Icons;

typedef struct{
	bool invert_dir;
	char *label1, *label2;
} SliderConfig;

extern AppConfig appConfig;
extern Icons icons;
extern SliderConfig sliderConfig;

void loadConfig(void);

#endif
