#pragma once

#include <glib.h>
#include <stdbool.h>

/**
 * @brief Application configuration structure
 */
typedef struct {
	char *verticalAlign, *horizontalAlign, *orientation;
	int timeOut, x, y, margin;
} AppConfig;

/**
 * @brief Icon configuration for different actions
 */
typedef struct {
	char *backlight, *volume, *volumeMute, *mic, *micMute;
	char *batLow, *batFull, *batCharge, *batDischarge, *batIdel, *batCrit, *wifi, *ethernet, *bluetooth;
} Icons;

/**
 * @brief Slider widget configuration
 */
typedef struct {
	bool invert_dir;
	char *label1, *label2;
} SliderConfig;

/**
 * @brief Text/label widget configuration
 */
typedef struct {
	char *label;
} TextConfig;

/** @brief Global application configuration */
extern AppConfig appConfig;
/** @brief Global icon configuration */
extern Icons icons;
/** @brief Global slider configuration */
extern SliderConfig sliderConfig;
/** @brief Global text configuration */
extern TextConfig textConfig;

/**
 * @brief Load configuration from config file
 */
void loadConfig(void);

/**
 * @brief Replace all occurrences of a substring in a string
 * @param src Source string
 * @param find Substring to find
 * @param replace Replacement string
 * @return Newly allocated string with replacements, caller must free
 */
char *strReplace(const char *src, const char *find, const char *replace);
