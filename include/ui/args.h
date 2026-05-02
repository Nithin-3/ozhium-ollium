#ifndef UI_ARGS_H
#define UI_ARGS_H

#include "common.h"

typedef struct {
	GUI_ELEMENT element;
	float min, max, current;
	union {
		SLIDER_ACTION slider;
		TEXT_ACTION text;
	} action;
	char text[256];
} ParsedArgs;

GUI_ELEMENT argToElement(const char *s);
SLIDER_ACTION argToSliderAction(const char *s);
TEXT_ACTION argToTextAction(const char *s);
int parseArgs(int argc, char *argv[], ParsedArgs *args);

#endif
