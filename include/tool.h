#pragma once

#include <stddef.h>
char *findConfigPath(const char *filename);
const char *findSelfDir(void);
int findBacklightPaths(char *bri_path, char *max_path, size_t size);
int catFile(const char *p, int *v);
int catFileStr(const char *p, char *v, size_t maxLen);
