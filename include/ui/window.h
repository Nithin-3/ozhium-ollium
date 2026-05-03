#pragma once

#include <gtk/gtk.h>

extern GtkWidget *globalWindow;

void applyLayerShell(GtkWindow *win);
void setupCss(void);
void resetTimer(int sec);
