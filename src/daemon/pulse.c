/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * pulse.c - PulseAudio integration
 */

#include "daemon/invoke.h"
#include "daemon/pulse.h"
#include <pulse/pulseaudio.h>
#include <stdio.h>

pa_context *pa_ctx;

static void sink_cb(pa_context *c, const pa_sink_info *info, int eol,void *ud) {
	(void)c;
	(void)ud;
	if (eol > 0)
		return;
	pa_volume_t vol = pa_cvolume_avg(&info->volume);
	float current = (float)vol / PA_VOLUME_NORM;
	int muted = info->mute;
	sliderData s = {
		.min=0,
		.max=1,
		.current=current,
		.action=muted ? AUDIO_MUTE : AUDIO,
	};
	execUI(SLIDER, &s);
}

static void source_cb(pa_context *c, const pa_source_info *info, int eol, void *ud) {
	(void)c;
	(void)ud;
	if (eol > 0)
		return;
	pa_volume_t vol = pa_cvolume_avg(&info->volume);
	float current = (float)vol / PA_VOLUME_NORM;
	int muted = info->mute;
	sliderData s = {
		.min=0,
		.max=1,
		.current=current,
		.action=muted ? MIC_MUTE : MIC,
	};
	execUI(SLIDER, &s);
}

static void event_cb(pa_context *c, pa_subscription_event_type_t t,uint32_t idx, void *ud) {
	(void)ud;
	int facility = t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
	int type     = t & PA_SUBSCRIPTION_EVENT_TYPE_MASK;
	if (facility == PA_SUBSCRIPTION_EVENT_SINK && type == PA_SUBSCRIPTION_EVENT_CHANGE) {
		pa_context_get_sink_info_by_index(c, idx, sink_cb, NULL);
	}
	if (facility == PA_SUBSCRIPTION_EVENT_SOURCE && type == PA_SUBSCRIPTION_EVENT_CHANGE) {
		pa_context_get_source_info_by_index(c, idx, source_cb, NULL);
	}
}

static void context_state_cb(pa_context *c, void *ud) {
	(void)ud;
	if (pa_context_get_state(c) != PA_CONTEXT_READY)
		return;
	pa_context_set_subscribe_callback(c, event_cb, NULL);
	pa_context_subscribe(c, PA_SUBSCRIPTION_MASK_SINK | PA_SUBSCRIPTION_MASK_SOURCE, NULL, NULL);
}

int initPulseAudio(pa_mainloop_api *api) {
	pa_ctx = pa_context_new(api, "pulse-sink-watcher");
	if (!pa_ctx) {
		fprintf(stderr, "Failed to create PA context\n");
		return 1;
	}
	pa_context_set_state_callback(pa_ctx, context_state_cb, NULL);
        if (0 > pa_context_connect(pa_ctx, NULL, PA_CONTEXT_NOFLAGS, NULL)) {
          fprintf(stderr, "PA connect failed: %s\n", pa_strerror(pa_context_errno(pa_ctx)));
          pa_context_unref(pa_ctx);
          return 1;
        }
        return 0;
}