#pragma once

#include <stddef.h>
char *findConfigPath(const char *filename);
const char *findSelfDir(void);
int catFile(const char *p, int *v);
int catFileStr(const char *p, char *v, size_t maxLen);
