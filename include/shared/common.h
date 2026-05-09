#pragma once

typedef enum { SLIDER,
	       TEXT } GUI_ELEMENT;
typedef enum { INVALID = -1,
	       BACKLIGHT,
	       AUDIO,
	       AUDIO_MUTE,
	       MIC,
	       MIC_MUTE,
	       BAT_CRIT,
	       BAT_LOW,
	       BAT_FULL,
	       BAT_CHARGE,
	       BAT_DISCHARGE,
	       BAT_IDEL,
	       WIFI,
	       ETHERNET,
	       BLUETOOTH } ACTION;

typedef struct {
	float min;
	float max;
	float current;
	ACTION action;
} sliderData;

typedef struct {
	char text[256];
	ACTION action;
} textData;

char *findConfigPath(const char *filename);

/**
 * @brief Replace all occurrences of a substring in a string
 * @param src Source string
 * @param find Substring to find
 * @param replace Replacement string
 * @return Newly allocated string with replacements, caller must free
 */
char *strReplace(const char *src, const char *find, const char *replace);
