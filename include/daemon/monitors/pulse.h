/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * pulse.h - PulseAudio monitoring
 */
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