/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * invoke.c - UI invocation
 */

#include "daemon/invoke.h"
#include "daemon/utils/tool.h"
#include "shared/common.h"
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


/*
 * actionToString , elementToString
 * it used for IPC
 * send string args
 * ui element and action
 */

const char *actionToString(ACTION a);
static const char *elementToString(GUI_ELEMENT e);

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

void execUI(const GUI_ELEMENT element, void *data) {
	initSigchld();

	const char *selfPath = findSelfDir();
	char uiBinary[PATH_MAX];
	snprintf(uiBinary, sizeof(uiBinary), "%s/ozhium-ollium-ui", selfPath);

	char elementBuf[8];
	strncpy(elementBuf, elementToString(element), sizeof(elementBuf));

	fprintf(stderr, "[execUI] forking...\n");
	pid_t pid = fork(); // cloning daemon....
	if (pid < 0) {
		perror("fork failed");
		return;
	}

	if (pid == 0) { // child process
		switch (element) {
			case SLIDER: {
					     sliderData *s = (sliderData *)data;
					     char minBuf[64], maxBuf[64], curBuf[64], actionBuf[8];
					     snprintf(minBuf, sizeof(minBuf), "%.6f", s->min);
					     snprintf(maxBuf, sizeof(maxBuf), "%.6f", s->max);
					     snprintf(curBuf, sizeof(curBuf), "%.6f", s->current);
					     strncpy(actionBuf, actionToString(s->action), sizeof(actionBuf));
					     char *args[] = {uiBinary, "--element", elementBuf, "--min", minBuf,   "--max",     maxBuf,     "--current", curBuf,   "--action",  actionBuf,  NULL};
					     execv(uiBinary, args); // clear daemon load uiBinary
					     break;
				     }
			case TEXT: {
				   textData *t = (textData *)data;
				   char actionBuf[8];
				   strncpy(actionBuf, actionToString(t->action), sizeof(actionBuf));
				   char *args[] = {uiBinary, "--element", elementBuf, "--text", t->text, "--action", actionBuf, NULL};
				   execv(uiBinary, args); // clear daemon load uiBinary
				   break;
			   }
		}
		perror("execv ozhium-ollium-ui"); // if fail to load uiBinary
		_exit(1);
	}
	fprintf(stderr, "[execUI] parent: child pid=%d\n", pid);
}


const char *actionToString(ACTION a) {
	switch (a) {
		case BACKLIGHT:
			return "s0";
		case AUDIO:
			return "s1";
		case AUDIO_MUTE:
			return "s2";
		case MIC:
			return "s3";
		case MIC_MUTE:
			return "s4";
		case BAT_LOW:
			return "t0";
		case BAT_FULL:
			return "t1";
		case BAT_CHARGE:
			return "t2";
		case BAT_DISCHARGE:
			return "t3";
		case BAT_IDEL:
			return "t4";
		case WIFI:
			return "t5";
		case ETHERNET:
			return "t6";
		case BLUETOOTH:
			return "t7";
		default:
			return "?";
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
