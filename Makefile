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
#     - libudev (udev library) - for hardware device monitoring
#     - libpulse (PulseAudio library) - for audio volume monitoring
#   
#   UI (ozhium-ollium-ui):
#     - GTK4 (GTK+ 4) - for GUI
#     - gtk4-layer-shell - for on-screen display (OSD) rendering
#
# System Requirements:
#   - Linux kernel with inotify support
#   - PulseAudio sound server
#   - udev device manager
#
# Installation: See DEPENDENCIES file for detailed setup instructions

CC      = gcc
CFLAGS  = -Wall -Wextra -I include

# GTK4 and gtk4-layer-shell dependencies for UI compilation
GTK_CFLAGS = $(shell pkg-config --cflags gtk4 gtk4-layer-shell-0)
GTK_LDFLAGS = $(shell pkg-config --libs gtk4 gtk4-layer-shell-0)

TARGET      = ozhium-ollium
UI_TARGET   = ozhium-ollium-ui

OBJ_DIR = out

DAEMON_SRC = src/daemon/ozhium-ollium.c src/daemon/invoke.c src/daemon/battery.c src/daemon/pulse.c src/daemon/tool.c src/daemon/backLight.c src/daemon/netlink.c
UI_SRC = src/ui/main.c src/ui/config.c src/ui/window.c src/ui/builder.c src/ui/args.c src/ui/tool.c

.PHONY: all clean

all: $(TARGET) $(UI_TARGET)

# Daemon target - links with libudev and libpulse
$(TARGET): $(OBJ_DIR)/daemon/ozhium-ollium.o $(OBJ_DIR)/daemon/invoke.o $(OBJ_DIR)/daemon/battery.o $(OBJ_DIR)/daemon/pulse.o $(OBJ_DIR)/daemon/tool.o $(OBJ_DIR)/daemon/backLight.o $(OBJ_DIR)/daemon/netlink.o
	$(CC) $^ -o $@ -lpulse

# UI target - links with GTK4 and gtk4-layer-shell
$(UI_TARGET): $(OBJ_DIR)/ui/main.o $(OBJ_DIR)/ui/config.o $(OBJ_DIR)/ui/window.o $(OBJ_DIR)/ui/builder.o $(OBJ_DIR)/ui/args.o $(OBJ_DIR)/ui/tool.o
	$(CC) $^ -o $@ $(GTK_LDFLAGS)

$(OBJ_DIR)/daemon/ozhium-ollium.o: src/daemon/ozhium-ollium.c | $(OBJ_DIR)/daemon
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/daemon/invoke.o: src/daemon/invoke.c | $(OBJ_DIR)/daemon
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/daemon/battery.o: src/daemon/battery.c | $(OBJ_DIR)/daemon
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/daemon/pulse.o: src/daemon/pulse.c | $(OBJ_DIR)/daemon
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/daemon/tool.o: src/daemon/tool.c | $(OBJ_DIR)/daemon
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/daemon/backLight.o: src/daemon/backLight.c | $(OBJ_DIR)/daemon
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/daemon/netlink.o: src/daemon/netlink.c | $(OBJ_DIR)/daemon
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/ui/%.o: src/ui/%.c | $(OBJ_DIR)/ui
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/daemon:
	mkdir -p $@

$(OBJ_DIR)/ui:
	mkdir -p $@

-include $(OBJ_DIR)/**/*.d

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(UI_TARGET)
