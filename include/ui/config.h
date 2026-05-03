#pragma once

#include <glib.h>
#include <stdbool.h>

typedef struct {
	char *verticalAlign, *horizontalAlign, *orientation;
	int timeOut, x, y, margin;
} AppConfig;

typedef struct {
	char *backlight, *volume, *volumeMute, *mic, *micMute;
	char *batLow, *batFull, *batCharge, *batDischarge, *batIdel;
} Icons;

typedef struct{
	bool invert_dir;
	char *label1, *label2;
} SliderConfig;

typedef struct{
	char *label;
} TextConfig;

extern AppConfig appConfig;
extern Icons icons;
extern SliderConfig sliderConfig;
extern TextConfig textConfig;

void loadConfig(void);
