#pragma once

#include "shared/common.h"
#include <gtk/gtk.h>

GtkWidget *buildSlider(const sliderData *s);
GtkWidget *buildText(const textData *t);
void updateContent(GUI_ELEMENT el, const sliderData *s, const textData *t);
