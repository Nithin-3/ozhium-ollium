#include "ui/args.h"
#include "shared/common.h"
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

GUI_ELEMENT argToElement(const char *s) {
	if (strcmp(s, "0") == 0 || strcmp(s, "slider") == 0) return SLIDER;
	return TEXT;
}

SLIDER_ACTION argToSliderAction(const char *s) {
	if (strcmp(s, "s0") == 0 || strcmp(s, "backlight") == 0) return BACKLIGHT;
	if (strcmp(s, "s1") == 0 || strcmp(s, "audio") == 0) return AUDIO;
	if (strcmp(s, "s2") == 0 || strcmp(s, "audio_mute") == 0) return AUDIO_MUTE;
	if (strcmp(s, "s3") == 0 || strcmp(s, "mic") == 0) return MIC;
	if (strcmp(s, "s4") == 0 || strcmp(s, "mic_mute") == 0) return MIC_MUTE;
	return BACKLIGHT;
}

TEXT_ACTION argToTextAction(const char *s) {
	if (strcmp(s, "t0") == 0 || strcmp(s, "bat_low") == 0) return BAT_LOW;
	if (strcmp(s, "t1") == 0 || strcmp(s, "bat_full") == 0) return BAT_FULL;
	if (strcmp(s, "t2") == 0 || strcmp(s, "bat_charge") == 0) return BAT_CHARGE;
	if (strcmp(s, "t3") == 0 || strcmp(s, "bat_discharge") == 0) return BAT_DISCHARGE;
	if (strcmp(s, "t4") == 0 || strcmp(s, "bat_idel") == 0) return BAT_IDEL;
	return BAT_LOW;
}

int parseArgs(int argc, char *argv[], ParsedArgs *args) {
	optind = 1;

	static struct option longOpts[] = {
		{ "element", required_argument, NULL, 'e' },
		{ "min", required_argument, NULL, 'm' },
		{ "max", required_argument, NULL, 'M' },
		{ "current", required_argument, NULL, 'c' },
		{ "action", required_argument, NULL, 'a' },
		{ "text", required_argument, NULL, 't' },
		{ NULL, 0, NULL, 0 }
	};

	int opt;
	while ((opt = getopt_long(argc, argv, "e:m:M:c:a:t:", longOpts, NULL)) != -1) {
		switch (opt) {
			case 'e':
				args->element = argToElement(optarg);
				break;
			case 'm':
				args->min = atof(optarg);
				break;
			case 'M':
				args->max = atof(optarg);
				break;
			case 'c':
				args->current = atof(optarg);
				break;
			case 'a':
				if (args->element == SLIDER) {
					args->action.slider = argToSliderAction(optarg);
				} else {
					args->action.text = argToTextAction(optarg);
				}
				break;
			case 't':
				strncpy(args->text, optarg, sizeof(args->text) - 1);
				args->text[sizeof(args->text) - 1] = '\0';
				break;
			default:
				return 1;
		}
	}

	return 0;
}