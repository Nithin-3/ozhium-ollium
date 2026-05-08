# ozhium-ollium Makefile
# 
# Project: A daemon that monitors Linux backlight and PulseAudio volume changes
#          and triggers a UI callback when they occur
#
# Build Requirements:
#   - GCC compiler
#   - pkg-config
#
# Runtime Dependencies:
#   DAEMON (ozhium-ollium):
#     - libpulse (PulseAudio library) - for audio volume monitoring
#   
#   UI (ozhium-ollium-ui):
#     - GTK4 (GTK+ 4) - for GUI
#     - gtk4-layer-shell - for on-screen display (OSD) rendering
#
# System Requirements:
#   - Linux kernel with inotify support
#   - PulseAudio sound server
#
# Installation: See DEPENDENCIES file for detailed setup instructions

CC      = gcc
CFLAGS  = -Wall -Wextra -I include
GTK_CFLAGS  = $(shell pkg-config --cflags gtk4 gtk4-layer-shell-0)
GTK_LDFLAGS = $(shell pkg-config --libs gtk4 gtk4-layer-shell-0)

TARGET    = ozhium-ollium
UI_TARGET = ozhium-ollium-ui
OBJ_DIR   = out

DAEMON_SRC = \
	src/daemon/ozhium-ollium.c \
	src/daemon/invoke.c \
	src/daemon/utils/battery.c \
	src/daemon/utils/tool.c \
	src/daemon/utils/backlight.c \
	src/daemon/monitors/pulse.c \
	src/daemon/monitors/netlink.c \
	src/daemon/monitors/inotify.c

UI_SRC = \
	src/ui/main.c \
	src/ui/config.c \
	src/ui/window.c \
	src/ui/builder.c \
	src/ui/args.c \
	src/ui/tool.c

# Auto-generate object paths from sources
DAEMON_OBJ = $(DAEMON_SRC:src/%.c=$(OBJ_DIR)/%.o)
UI_OBJ     = $(UI_SRC:src/%.c=$(OBJ_DIR)/%.o)

# Auto-generate dependency files
DEPS = $(DAEMON_OBJ:.o=.d) $(UI_OBJ:.o=.d)

.PHONY: all clean format check-format

all: $(TARGET) $(UI_TARGET)

# Daemon — links with libpulse
$(TARGET): $(DAEMON_OBJ)
	$(CC) $^ -o $@ -lpulse

# UI — links with GTK4 and gtk4-layer-shell
$(UI_TARGET): $(UI_OBJ)
	$(CC) $^ -o $@ $(GTK_LDFLAGS)

# Daemon object files
$(OBJ_DIR)/daemon/%.o: src/daemon/%.c | $(OBJ_DIR)/daemon \
                                         $(OBJ_DIR)/daemon/utils \
                                         $(OBJ_DIR)/daemon/monitors
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# UI object files
$(OBJ_DIR)/ui/%.o: src/ui/%.c | $(OBJ_DIR)/ui
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -MMD -MP -c $< -o $@

# Create output directories
$(OBJ_DIR)/daemon $(OBJ_DIR)/daemon/utils $(OBJ_DIR)/daemon/monitors $(OBJ_DIR)/ui:
	mkdir -p $@

# Include dependency files
-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(UI_TARGET)

format:
	find src/ include/ -name "*.[ch]" | xargs clang-format -i

check-format:
	find src/ include/ -name "*.[ch]" | xargs clang-format --dry-run --Werror
