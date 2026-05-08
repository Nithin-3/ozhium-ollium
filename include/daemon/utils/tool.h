#pragma once

#include <stddef.h>

/**
 * @brief Find the configuration file path
 * @param filename The configuration filename to search for
 * @return Full path to the config file, or NULL if not found
 */
char *findConfigPath(const char *filename);

/**
 * @brief Get the directory path where the executable is located
 * @return Directory path of the executable
 */
const char *findSelfDir(void);

/**
 * @brief Read an integer value from a file
 * @param p Path to the file
 * @param v Pointer to store the read integer value
 * @return 1 on success, 0 on failure
 */
int catFile(const char *p, int *v);

/**
 * @brief Read a string value from a file
 * @param p Path to the file
 * @param v Buffer to store the read string
 * @param maxLen Maximum length of the buffer
 * @return 1 on success, 0 on failure
 */
int catFileStr(const char *p, char *v, size_t maxLen);