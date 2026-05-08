#pragma once

#include <pulse/mainloop.h>
#include <stddef.h>

int initInotify(pa_mainloop_api *api);
void cleanupInotify(void);