#include "backLightTool.h"
#include "batteryTool.h"
#include "invoke.h"
#include "pulseTool.h"
#include "tool.h"
#include <dirent.h>
#include <libudev.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>


int bri_wd=0;
static struct udev_monitor *udev_mon = NULL;

static void inotify_cb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
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
			if (ev->wd == bri_wd) {
				sliderData slider;
				if (0 == getBacklight(&slider)) execUI(SLIDER, &slider);
			}
		}
		p += sizeof(struct inotify_event) + ev->len;
	}
}

static void udev_cb(pa_mainloop_api *api, pa_io_event *e, int fd, pa_io_event_flags_t events, void *ud) {
	(void)api; (void)e; (void)fd; (void)events; (void)ud;

	if (!(events & PA_IO_EVENT_INPUT)) return;

	struct udev_device *dev = udev_monitor_receive_device(udev_mon);
	if (!dev) return;

	const char *power_type = udev_device_get_property_value(dev, "POWER_SUPPLY_TYPE");
	if (!power_type || strcmp(power_type, "Battery") != 0) {
		udev_device_unref(dev);
		return;
	}

	const char *action = udev_device_get_action(dev);
	const char *status = udev_device_get_sysattr_value(dev, "status");
	if (action && (0 == strcmp(action, "change") || 0 == strcmp(action, "update"))) {

		char buff[32];
		if (!catFileStr(batSta_path, buff, sizeof(buff))) return;
		if (0 == strcmp(status, buff)) return;
		textData t = {0};
		getBattery(&t);

		execUI(TEXT, &t);
	}
        udev_device_unref(dev);
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
	bri_wd = inotify_add_watch(in_fd, bri_path, IN_MODIFY);
	if (bri_wd < 0) { perror("inotify watch"); close(in_fd); pa_mainloop_free(ml); return 1; }
	printf("[inotify watch] path : %s\n", bri_path);

	api->io_new(api, in_fd, PA_IO_EVENT_INPUT, inotify_cb, NULL);

	struct udev *udev = udev_new();
	if (!udev) { fprintf(stderr, "Failed to create udev\n"); }
	else {
		udev_mon = udev_monitor_new_from_netlink(udev, "kernel");
		if (udev_mon) {
			udev_monitor_filter_add_match_subsystem_devtype(udev_mon, "power_supply", NULL);
			udev_monitor_enable_receiving(udev_mon);
			int udev_fd = udev_monitor_get_fd(udev_mon);
			printf("[udev] monitoring power_supply\n");
			api->io_new(api, udev_fd, PA_IO_EVENT_INPUT, udev_cb, NULL);
		}
	}


	if (initPulseAudio(api) != 0) {
		fprintf(stderr, "Failed to init PulseAudio\n");
		close(in_fd);
		pa_mainloop_free(ml);
		return 1;
	}

	printf("watch pulse srink source\n(Ctrl+C to exit)\n");
	pa_mainloop_run(ml, NULL);

	if (pa_ctx) pa_context_unref(pa_ctx);
	close(in_fd);
	if (udev_mon) udev_monitor_unref(udev_mon);
	if (udev) udev_unref(udev);
	pa_mainloop_free(ml);
	return 0;
}
