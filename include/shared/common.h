#pragma once

typedef enum { SLIDER, TEXT } GUI_ELEMENT;
typedef enum { BACKLIGHT, AUDIO, AUDIO_MUTE, MIC, MIC_MUTE } SLIDER_ACTION;
typedef enum { BAT_LOW, BAT_FULL, BAT_CHARGE, BAT_DISCHARGE, BAT_IDEL } TEXT_ACTION;

typedef struct {
	float min;
	float max;
	float current;
	SLIDER_ACTION action;
} sliderData;

typedef struct {
	char text[256];
	TEXT_ACTION action;
} textData;
