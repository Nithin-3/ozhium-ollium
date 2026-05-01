#include "invoke.h"
#include "tool.h"
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void reapChildren(int sig) {
	(void)sig;
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

static void initSigchld(void) {
	static int done = 0;
	if (!done) {
		struct sigaction sa = {0};
		sa.sa_handler = reapChildren;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
		sigaction(SIGCHLD, &sa, NULL);
		done = 1;
	}
}

static const char *actionToString(SLIDER_ACTION a) {
	switch (a) {
		case BACKLIGHT:
			return "0";
		case AUDIO:
			return "1";
		case AUDIO_MUTE:
			return "2";
		case MIC:
			return "3";
		case MIC_MUTE:
			return "4";
		default:
			return "0";
	}
}

static const char *elementToString(GUI_ELEMENT e) {
	switch (e) {
		case SLIDER:
			return "0";
		case TEXT:
			return "1";
		default:
			return "0";
	}
}

void execUI(const GUI_ELEMENT element, void *data) {
	fprintf(stderr, "[execUI] START: element=%d\n", element);
	initSigchld();

	const char *selfPath = findSelfDir();
	char uiBinary[PATH_MAX];
	snprintf(uiBinary, sizeof(uiBinary), "%s/ozhium-ollium-ui", selfPath);

	char elementBuf[8];
	strncpy(elementBuf, elementToString(element), sizeof(elementBuf));

	fprintf(stderr, "[execUI] forking...\n");
	pid_t pid = fork();
	if (pid < 0) {
		perror("fork failed");
		return;
	}

	if (pid == 0) {
		switch (element) {
			case SLIDER: {
					     sliderData *s = (sliderData *)data;
					     fprintf(
							     stderr,
							     "[execUI child] SLIDER: min=%.2f max=%.2f current=%.2f action=%d\n",
							     s->min, s->max, s->current, s->action);
					     char minBuf[64], maxBuf[64], curBuf[64], actionBuf[8];
					     snprintf(minBuf, sizeof(minBuf), "%.6f", s->min);
					     snprintf(maxBuf, sizeof(maxBuf), "%.6f", s->max);
					     snprintf(curBuf, sizeof(curBuf), "%.6f", s->current);
					     strncpy(actionBuf, actionToString(s->action), sizeof(actionBuf));

					     char *args[] = {uiBinary, "--element", elementBuf, "--min",
						     minBuf,   "--max",     maxBuf,     "--current",
						     curBuf,   "--action",  actionBuf,  NULL};
					     fprintf(stderr,
							     "[execUI execv] Command: %s --element %s --min %s --max %s "
							     "--current %s --action %s\n",
							     uiBinary, elementBuf, minBuf, maxBuf, curBuf, actionBuf);
					     execv(uiBinary, args);
					     break;
				     }
			case TEXT: {
					   textData *t = (textData *)data;
					   fprintf(stderr, "[execUI child] TEXT: text=%s\n", t->text);
					   char *args[] = {uiBinary, "--element", elementBuf,
						   "--text", t->text,     NULL};
					   execv(uiBinary, args);
					   break;
				   }
		}
		perror("execv ozhium-ollium-ui");
		_exit(1);
	}
	fprintf(stderr, "[execUI] parent: child pid=%d\n", pid);
	fprintf(stderr, "[execUI] END\n");
}
