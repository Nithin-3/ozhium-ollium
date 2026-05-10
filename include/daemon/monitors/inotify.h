#pragma once

#include <pulse/mainloop.h>
#include <stddef.h>

/**
 * @brief Initialize inotify monitoring for configuration file changes
 * @param api Pointer to the PulseAudio mainloop API
 * @return 0 on success, non-zero on failure
 */
int initInotify(pa_mainloop_api *api);

/**
 * @brief Cleanup and close inotify file descriptor and associated resources
 */
void cleanupInotify(void);