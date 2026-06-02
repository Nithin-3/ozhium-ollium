/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * invoke.c - UI invocation
 */

#include "daemon/invoke.h"
#include "daemon/config.h"
#include "shared/log.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void childRedirect(void) {
	int logFd = logGetFd();
	if (logFd >= 0) {
		dup2(logFd, STDOUT_FILENO);
		dup2(logFd, STDERR_FILENO);
	} else {
		FILE *null = fopen("/dev/null", "w");
		if (null) {
			dup2(fileno(null), STDOUT_FILENO);
			dup2(fileno(null), STDERR_FILENO);
			fclose(null);
		}
	}
}

static void reapChildren(int sig) {
	(void)sig;
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
}

static void initSigchld(void) {
	static int done = 0;
	if (!done) {
		struct sigaction sa = { 0 };
		sa.sa_handler = reapChildren;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
		sigaction(SIGCHLD, &sa, NULL);
		done = 1;
	}
}

void execUI(const ACTION action, void *data) {
	initSigchld();
	logInfo("[execUI] forking...");
	pid_t pid = fork();
	if (pid < 0) {
		logError("fork failed: %s", strerror(errno));
		return;
	}
	if (pid == 0) {
		childRedirect();
		char **args = daemonExec(action, data);
		if (args) {
			execvp(args[0], args);
			logError("execvp: %s", strerror(errno));
		}
		_exit(1);
	}

	// Parent continues here
	if (!isDaemonNativeExec(action))
		return;

	pid_t pid2 = fork();
	if (pid2 < 0) {
		logError("fork failed (native exec): %s", strerror(errno));
		return;
	}
	if (pid2 != 0)
		return;	 // parent returns

	// Second child
	childRedirect();
	char **args = daemonNativeExec(action, data);
	if (args) {
		execvp(args[0], args);
		logError("execvp: %s", strerror(errno));
	}
	_exit(1);
}
