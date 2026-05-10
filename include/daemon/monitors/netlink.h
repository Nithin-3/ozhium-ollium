/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * netlink.h - netlink socket monitoring
 */
#pragma once

#include <pulse/mainloop-api.h>

/**
 * @brief Initialize netlink socket monitoring for network and uevent changes
 * @param api Pointer to the PulseAudio mainloop API
 * @return 0 on success, non-zero on failure
 */
int initNetlink(pa_mainloop_api *api);

/**
 * @brief Cleanup and close netlink sockets and associated resources
 */
void cleanupNetlink(void);
