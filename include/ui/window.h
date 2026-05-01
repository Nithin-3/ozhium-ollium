#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <gtk/gtk.h>

extern GtkWidget *globalWindow;

void applyLayerShell(GtkWindow *win);
void setupCss(void);
void resetTimer(int sec);

#endif
