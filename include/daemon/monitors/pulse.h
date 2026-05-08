#pragma once

#include <pulse/pulseaudio.h>

/** @brief Global PulseAudio context */
extern pa_context *pa_ctx;

/**
 * @brief Initialize PulseAudio connection and event monitoring
 * @param api Pointer to the PulseAudio mainloop API
 * @return 0 on success, non-zero on failure
 */
int initPulseAudio(pa_mainloop_api *api);