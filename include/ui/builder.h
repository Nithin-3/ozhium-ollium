#ifndef UI_BUILDER_H
#define UI_BUILDER_H

#include "invoke.h"
#include <gtk/gtk.h>

extern sliderData sliderArgs;
extern textData textArgs;

GtkWidget *buildSlider(void);
GtkWidget *buildText(void);
void updateContent(GUI_ELEMENT el, const sliderData *s, const textData *t);

#endif
