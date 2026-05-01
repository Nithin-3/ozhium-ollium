#ifndef UI_BUILDER_H
#define UI_BUILDER_H

#include "invoke.h"
#include <gtk/gtk.h>

extern sliderData sliderArgs;
extern textData textArgs;

GtkWidget *buildSlider(void);
GtkWidget *buildText(void);

#endif
