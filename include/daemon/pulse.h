#pragma once

#include <pulse/pulseaudio.h>

extern pa_context *pa_ctx;

int initPulseAudio(pa_mainloop_api *api);
