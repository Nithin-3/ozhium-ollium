#ifndef INVOKE_H
#define INVOKE_H
typedef enum {SLIDER} GUI_ELEMENT;
typedef enum {BACKLIGHT,AUDIO,AUDIO_MUTE} SLIDER_ACTION;

typedef struct {
    float min;
    float max;
    float current;
    SLIDER_ACTION action;
} sliderData;

void showUI(const GUI_ELEMENT element,void *data);

#endif

