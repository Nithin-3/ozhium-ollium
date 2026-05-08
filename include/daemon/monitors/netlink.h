#pragma once

#include <pulse/mainloop-api.h>
int initNetlink(pa_mainloop_api *api);
void cleanupNetlink(void);
