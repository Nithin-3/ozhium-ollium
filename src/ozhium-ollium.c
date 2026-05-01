#include "backLightTool.h"
#include "invoke.h"
#include "pulseTool.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>

static void backlight_cb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api;
	(void)e;
	(void)ud;
	if (!(events & PA_IO_EVENT_INPUT)) return;
	char buf[4096];
	ssize_t len = read(fd, buf, sizeof(buf));
	if (len <= 0) return;
	for (char *p = buf; p < buf + len; ) {
		struct inotify_event *ev = (struct inotify_event *)p;
		if (ev->mask & IN_MODIFY) {
			sliderData slider;
			float bri = getBacklight(&slider);
			if (bri >= 0) execUI(SLIDER, &slider);
		}
		p += sizeof(struct inotify_event) + ev->len;
	}
}

int main() {
	printf("ozhium-ollium started ....\n");
	pa_mainloop *ml = pa_mainloop_new();
	if (!ml) { fprintf(stderr, "Failed to create mainloop\n"); return 1; }
	pa_mainloop_api *api = pa_mainloop_get_api(ml);

	if (findBacklightPaths(bri_path, max_path, PATH_MAX)) {
		fprintf(stderr, "could not find backlight path\n");
		pa_mainloop_free(ml);
		return 1;
	}

	int in_fd = inotify_init();
	if (in_fd < 0) { perror("inotify init"); pa_mainloop_free(ml); return 1; }
	int wd = inotify_add_watch(in_fd, bri_path, IN_MODIFY);
	if (wd < 0) { perror("inotify watch"); close(in_fd); pa_mainloop_free(ml); return 1; }
	api->io_new(api, in_fd, PA_IO_EVENT_INPUT, backlight_cb, NULL);

	if (initPulseAudio(api) != 0) {
		fprintf(stderr, "Failed to init PulseAudio\n");
		close(in_fd);
		pa_mainloop_free(ml);
		return 1;
	}

	printf("Watching audio + backlight changes... (Ctrl+C to exit)\n");
	pa_mainloop_run(ml, NULL);

	if (pa_ctx) pa_context_unref(pa_ctx);
	close(in_fd);
	pa_mainloop_free(ml);
	return 0;
}
