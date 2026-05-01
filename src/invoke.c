#include "invoke.h"
#include "tool.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static const char *actionToString(SLIDER_ACTION a) {
	switch (a) {
		case BACKLIGHT:    return "0";
		case AUDIO:        return "1";
		case AUDIO_MUTE:   return "2";
		default:           return "0";
	}
}

static const char *elementToString(GUI_ELEMENT e) {
	switch (e) {
		case SLIDER: return "0";
		case TEXT:   return "1";
		default:     return "0";
	}
}

void execUI(const GUI_ELEMENT element, void *data) {
	const char *selfPath = findSelfDir();
	char uiBinary[PATH_MAX];
	snprintf(uiBinary, sizeof(uiBinary), "%s/ozhium-ollium-ui", selfPath);

	char elementBuf[8];
	strncpy(elementBuf, elementToString(element), sizeof(elementBuf));

	pid_t pid = fork();
	if (pid < 0) {
		perror("fork failed");
		return;
	}

	if (pid == 0) {
		switch (element) {
			case SLIDER: {
				sliderData *s = (sliderData *)data;
				char minBuf[64], maxBuf[64], curBuf[64], actionBuf[8];
				snprintf(minBuf, sizeof(minBuf), "%.6f", s->min);
				snprintf(maxBuf, sizeof(maxBuf), "%.6f", s->max);
				snprintf(curBuf, sizeof(curBuf), "%.6f", s->current);
				strncpy(actionBuf, actionToString(s->action), sizeof(actionBuf));

				char *args[] = {
					uiBinary,
					"--element", elementBuf,
					"--min", minBuf,
					"--max", maxBuf,
					"--current", curBuf,
					"--action", actionBuf,
					NULL
				};
				execvp(uiBinary, args);
				break;
			}
			case TEXT: {
				textData *t = (textData *)data;
				char *args[] = {
					uiBinary,
					"--element", elementBuf,
					"--text", t->text,
					NULL
				};
				execvp(uiBinary, args);
				break;
			}
		}
		perror("execvp ozhium-ollium-ui");
		_exit(1);
	}
}
