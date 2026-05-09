#pragma once

#include "shared/common.h"
#include <stddef.h>

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


const char *actionToString(ACTION a);
const char *elementToString(GUI_ELEMENT e);

