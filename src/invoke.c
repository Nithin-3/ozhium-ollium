#include "invoke.h"
#include "osd.h"
#include <stdio.h>

void showUI(const GUI_ELEMENT element, void *data) {
	switch (element) {
		case SLIDER: 
			{
				sliderData *slider = (sliderData *)data;
				switch (slider->action) {
					case BACKLIGHT:
						printf("BACKLIGHT Slider: min=%.2f, max=%.2f, current=%.2f\n", slider->min, slider->max, slider->current);
						initApp(element,slider);
						break;
					case AUDIO:
						printf("AUDIO Slider: min=%.2f, max=%.2f, current=%.2f\n", slider->min, slider->max, slider->current);
						initApp(element,slider);
						break;
					case AUDIO_MUTE:
						printf("AUDIO_MUTE Slider: min=%.2f, max=%.2f, current=%.2f\n", slider->min, slider->max, slider->current);
						initApp(element,slider);
						break;
					default:
						perror("Unknown slider\n");break;
				}
				break;
			}
		default:
			     perror("Unknown GUI element\n");
			     break;
	}
}
