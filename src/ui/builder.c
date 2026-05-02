#include "ui/builder.h"
#include "ui/config.h"
#include "ui/config.h"
#include <stdio.h>
#include <string.h>

static GtkWidget *sliderWidget = NULL;
static GtkWidget *labelBefore = NULL;
static GtkWidget *labelAfter = NULL;
static GtkWidget *textWidget = NULL;


static char *getIconForAction(SLIDER_ACTION action) {
	switch (action) {
		case BACKLIGHT:
			return icons.backlight;
		case AUDIO:
			return icons.volume;
		case AUDIO_MUTE:
			return icons.volumeMute;
		case MIC:
			return icons.mic;
		case MIC_MUTE:
			return icons.micMute;
		default:
			return icons.backlight;
	}
}

static char *getIconForTextAction(TEXT_ACTION action) {
	switch (action) {
		case BAT_LOW:
			return icons.batLow;
		case BAT_FULL:
			return icons.batFull;
		case BAT_CHARGE:
			return icons.batCharge;
		case BAT_DISCHARGE:
			return icons.batDischarge;
		case BAT_IDEL:
			return icons.batIdel;
		default:
			return icons.batLow;
	}
}

char *str_replace(const char *src, const char *find, const char *replace) {
    char *result;
    const char *pos = strstr(src, find);
    if (!pos)
        return strdup(src); // not found, return copy as-is

    size_t find_len    = strlen(find);
    size_t replace_len = strlen(replace);
    size_t prefix_len  = pos - src;
    size_t suffix_len  = strlen(pos + find_len);

    result = malloc(prefix_len + replace_len + suffix_len + 1);
    if (!result) return NULL;

    memcpy(result, src, prefix_len);
    memcpy(result + prefix_len, replace, replace_len);
    memcpy(result + prefix_len + replace_len, pos + find_len, suffix_len + 1); // +1 for '\0'

    return result;
}

void applySlider(sliderData *s){
	gtk_range_set_value(GTK_RANGE(sliderWidget), (double)s->current);
	gchar *textBuf = g_strdup_printf("%d", (int)(1<s->current?s->current:s->current * 100));
	gtk_label_set_text(GTK_LABEL(labelBefore), str_replace(str_replace(sliderConfig.label1, "#ico#",getIconForAction(s->action)), "#val#", textBuf));
	gtk_label_set_text(GTK_LABEL(labelAfter), str_replace(str_replace(sliderConfig.label2, "#ico#",getIconForAction(s->action)), "#val#", textBuf));
	g_free(textBuf);
}

void applyText(textData *t){
	if (textWidget) {
		char *icon = getIconForTextAction(t->action);
		char labelText[512];
		snprintf(labelText, sizeof(labelText), "%s %s", icon ? icon : "", t->text);
		gtk_label_set_text(GTK_LABEL(textWidget), labelText);
	} else {
		printf("[APPLY_TEXT] WARNING: textWidget is NULL\n");
	}
}

GtkWidget *buildSlider(const sliderData *s) {
	printf("[buildSlider] Called with: min=%.2f, max=%.2f, current=%.2f, action=%d\n", s->min, s->max, s->current, s->action);
	
	GtkOrientation orient = GTK_ORIENTATION_HORIZONTAL;
	if (strcmp(appConfig.orientation, "vertical") == 0)
		orient = GTK_ORIENTATION_VERTICAL;

	sliderWidget = gtk_scale_new_with_range(orient, (double)s->min, (double)s->max, (double)(s->max / 100.0));
	gtk_scale_set_draw_value(GTK_SCALE(sliderWidget), FALSE);
	gtk_widget_set_name(sliderWidget, "slider");
	gtk_widget_set_hexpand(sliderWidget, TRUE);
	gtk_widget_set_vexpand(sliderWidget, TRUE);
	gtk_widget_set_sensitive(sliderWidget, FALSE);

	
	// Set minimum size for slider (horizontal: 100px width, vertical: 100px height)
	if (orient == GTK_ORIENTATION_HORIZONTAL) {
		gtk_widget_set_size_request(sliderWidget, 100, -1);
		gtk_range_set_inverted(GTK_RANGE(sliderWidget), sliderConfig.invert_dir);
	} else {
		gtk_widget_set_size_request(sliderWidget, -1, 100);
		gtk_range_set_inverted(GTK_RANGE(sliderWidget), !sliderConfig.invert_dir);
	}

	labelBefore = gtk_label_new("");
	labelAfter = gtk_label_new("");
	gtk_widget_set_name(labelBefore, "slider-label-before");
	gtk_widget_set_name(labelAfter, "slider-label-after");

	// Apply initial values
	sliderData *mutableSlider = (sliderData *)s;
	applySlider(mutableSlider);

	GtkWidget *box = gtk_box_new(orient, 6);
	
	gtk_widget_set_name(box, "box");
	gtk_widget_set_hexpand(box, TRUE);
	gtk_widget_set_vexpand(box, TRUE);
	gtk_box_append(GTK_BOX(box), labelBefore);
	gtk_box_append(GTK_BOX(box), sliderWidget);
	gtk_box_append(GTK_BOX(box), labelAfter);

	textWidget = NULL;

	return box;
}

GtkWidget *buildText(const textData *t) {
	printf("[buildText] Called with: text='%s' action=%d\n", t->text, t->action);
	
	char *icon = getIconForTextAction(t->action);
	char labelText[512];
	snprintf(labelText, sizeof(labelText), "%s %s", icon ? icon : "", t->text);
	
	textWidget = gtk_label_new(labelText);
	gtk_widget_set_name(textWidget, "text");
	gtk_label_set_xalign(GTK_LABEL(textWidget), 0.5);
	gtk_label_set_yalign(GTK_LABEL(textWidget), 0.5);
	gtk_widget_set_hexpand(textWidget, TRUE);
	gtk_widget_set_vexpand(textWidget, TRUE);

	sliderWidget = NULL;

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_widget_set_name(box, "box");
	gtk_box_append(GTK_BOX(box), textWidget);

	return box;
}

void updateContent(GUI_ELEMENT el, const sliderData *s, const textData *t) {
	printf("[updateContent] Called with element=%d\n", el);
	
	switch (el) {
		case SLIDER:
			if (sliderWidget) {
				printf("[updateContent] SLIDER: min=%.2f, max=%.2f, current=%.2f, action=%d\n", 
					s->min, s->max, s->current, s->action);
				
				sliderData *mutableSlider = (sliderData *)s;
				applySlider(mutableSlider);
			}
			break;
		case TEXT:
			if (textWidget) {
				printf("[updateContent] TEXT: text='%s'\n", t->text);
				
				textData *mutableText = (textData *)t;
				applyText(mutableText);
			}
			break;
	}
}
