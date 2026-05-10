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
