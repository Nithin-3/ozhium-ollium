#ifndef INVOKE_H
#define INVOKE_H

typedef enum { SLIDER, TEXT } GUI_ELEMENT;
typedef enum { BACKLIGHT, AUDIO, AUDIO_MUTE } SLIDER_ACTION;

typedef struct {
	float min;
	float max;
	float current;
	SLIDER_ACTION action;
} sliderData;

typedef struct {
	char text[256];
} textData;

void execUI(const GUI_ELEMENT element, void *data);

#endif
