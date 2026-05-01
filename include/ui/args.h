#ifndef UI_ARGS_H
#define UI_ARGS_H

#include "invoke.h"

typedef struct {
	GUI_ELEMENT element;
	float min, max, current;
	SLIDER_ACTION action;
	char *text;
} ParsedArgs;

int parseArgs(int argc, char *argv[], ParsedArgs *args);

#endif
