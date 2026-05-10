/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * config.h - Daemon configuration
 */
#pragma once

#include "shared/common.h"

/**
 * @brief Get a configuration value from the daemon config file
 * @param section The section name in the config file
 * @param key The configuration key to retrieve
 * @return String value of the configuration, or "" on failure
 */
char *daemonConfigGet(const char *section, const char *key);

/**
 * @brief Execute a daemon action with associated data
 * @param a The action to execute
 * @param data Pointer to action-specific data (sliderData or textData)
 * @return Array of strings (NULL-terminated), or NULL on failure
 */
char **daemonExec(ACTION a, void *data);

/**
 * @brief Execute a native daemon action with associated data
 * @param a The action to execute
 * @param data Pointer to action-specific data
 * @return Array of strings (NULL-terminated), or NULL on failure
 */
char **daemonNativeExec(ACTION a, void *data);

/**
 * @brief Check if an action requires native execution
 * @param a The action to check
 * @return 1 if native execution required, 0 otherwise
 */
int isDaemonNativeExec(ACTION a);
