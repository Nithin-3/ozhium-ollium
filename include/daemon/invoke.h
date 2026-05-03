#pragma once

#include "shared/common.h"

/**
 * @brief Execute the UI process with specified element and data
 * @param element The GUI element type (SLIDER or TEXT)
 * @param data Pointer to sliderData or textData depending on element type
 */
void execUI(const GUI_ELEMENT element, void *data);