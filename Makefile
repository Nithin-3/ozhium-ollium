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
#   UI/gtk (ozhium-ollium-ui):
#     - GTK4 (GTK+ 4) - for GUI
#     - gtk4-layer-shell - for on-screen display (OSD) rendering
#
#   UI/qt: Qt6 QML + DBus backend (selectable via UI_BACKEND=qt)
#
# System Requirements:
#   - Linux kernel with inotify support
#   - PulseAudio sound server
#
# Installation: See DEPENDENCIES file for detailed setup instructions

CC      = gcc
VERSION ?= 1.0.1
PREFIX  ?= /usr/local
CFLAGS  = -Wall -Wextra -fPIC -I include -I third_party/inih -DVERSION=\"$(VERSION)\"
GTK_CFLAGS  = $(shell pkg-config --cflags gtk4 gtk4-layer-shell-0)
GTK_LDFLAGS = $(shell pkg-config --libs gtk4 gtk4-layer-shell-0)
QT_CFLAGS   = $(shell pkg-config --cflags Qt6Core Qt6Gui Qt6Qml Qt6Quick Qt6DBus 2>/dev/null || pkg-config --cflags Qt5Core Qt5Gui Qt5Qml Qt5Quick 2>/dev/null)
QT_LDFLAGS  = $(shell pkg-config --libs Qt6Core Qt6Gui Qt6Qml Qt6Quick Qt6DBus 2>/dev/null || pkg-config --libs Qt5Core Qt5Gui Qt5Qml Qt5Quick 2>/dev/null)
CXX         = g++
UI_BACKEND ?= $(or $(UI),)

TARGET    = ozhium-ollium
UI_TARGET = ozhium-ollium-ui
OBJ_DIR   = out

DAEMON_SRC = \
	src/daemon/ozhium-ollium.c \
	src/daemon/invoke.c \
	src/daemon/config.c \
	src/daemon/utils/battery.c \
	src/daemon/utils/tool.c \
	src/daemon/utils/backlight.c \
	src/daemon/monitors/pulse.c \
	src/daemon/monitors/netlink.c \
	src/daemon/monitors/inotify.c \
	src/shared/common.c \
	src/shared/log.c

UI_GTK_SRC = \
	src/ui/gtk/main.c \
	src/ui/gtk/config.c \
	src/ui/gtk/window.c \
	src/ui/gtk/builder.c \
	src/ui/gtk/args.c \
	src/ui/gtk/tool.c \
	src/shared/common.c \
	src/shared/log.c

QT_UI_SRC = \
	src/ui/qt/main.cpp \
	src/ui/qt/args.cpp \
	src/ui/qt/dbus.cpp \
	src/ui/qt/dbusshim.cpp

INI_OBJ = $(OBJ_DIR)/third_party/inih/ini.o

# Auto-generate object paths from sources
DAEMON_OBJ = $(DAEMON_SRC:src/%.c=$(OBJ_DIR)/%.o)
UI_GTK_OBJ = $(UI_GTK_SRC:src/%.c=$(OBJ_DIR)/%.o) $(INI_OBJ)
QT_UI_OBJ  = $(QT_UI_SRC:src/%.cpp=$(OBJ_DIR)/%.o) $(OBJ_DIR)/shared/common.o $(OBJ_DIR)/shared/log.o $(INI_OBJ)

# Auto-generate dependency files
DEPS = $(DAEMON_OBJ:.o=.d) $(UI_GTK_OBJ:.o=.d)

.PHONY: all clean format check-format compile_commands install ui-gtk ui-qt $(UI_TARGET)

all: $(TARGET) $(UI_TARGET)

# Daemon — links with libpulse
$(TARGET): $(DAEMON_OBJ) $(INI_OBJ)
	$(CC) $^ -o $@ -lpulse

# UI — interactive backend selection
$(UI_TARGET):
ifeq ($(UI_BACKEND),qt)
	+$(MAKE) ui-qt
else ifeq ($(UI_BACKEND),gtk)
	+$(MAKE) ui-gtk
else
	@echo "Select UI backend for $(UI_TARGET):"; \
	echo "  1) GTK  (GTK4 + layer-shell)"; \
	echo "  2) Qt   (Qt6 QML + DBus)"; \
	read -p "Choice [1]: " c; \
	case "$$c" in \
		2|qt) $(MAKE) UI_BACKEND=qt ui-qt ;; \
		*)    $(MAKE) UI_BACKEND=gtk ui-gtk ;; \
	esac
endif

# GTK UI
ui-gtk: $(UI_GTK_OBJ)
	$(CC) $^ -o $(UI_TARGET) $(GTK_LDFLAGS)

# Qt UI — compiled via QDBusVirtualObject (no moc needed)
$(OBJ_DIR)/ui/qt/%.o: src/ui/qt/%.cpp | $(OBJ_DIR)/ui/qt
	$(CXX) $(CFLAGS) $(QT_CFLAGS) -MMD -MP -c $< -o $@

ui-qt: $(QT_UI_OBJ)
	$(CXX) $^ -o $(UI_TARGET) $(QT_LDFLAGS)

# Daemon object files
$(OBJ_DIR)/daemon/%.o: src/daemon/%.c | $(OBJ_DIR)/daemon \
                                         $(OBJ_DIR)/daemon/utils \
                                         $(OBJ_DIR)/daemon/monitors
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# UI object files
$(OBJ_DIR)/ui/gtk/%.o: src/ui/gtk/%.c | $(OBJ_DIR)/ui/gtk
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -MMD -MP -c $< -o $@

# Shared object files (used by both daemon and UI)
$(OBJ_DIR)/shared/%.o: src/shared/%.c | $(OBJ_DIR)/shared
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Third-party object files
$(OBJ_DIR)/third_party/inih/%.o: third_party/inih/%.c | $(OBJ_DIR)/third_party/inih
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Create output directories
$(OBJ_DIR)/daemon $(OBJ_DIR)/daemon/utils $(OBJ_DIR)/daemon/monitors $(OBJ_DIR)/ui/gtk $(OBJ_DIR)/ui/qt $(OBJ_DIR)/shared $(OBJ_DIR)/third_party/inih:
	mkdir -p $@

# Include dependency files
-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(UI_TARGET)

format:
	find src/ include/ -name "*.[ch]" | xargs clang-format -i

check-format:
	find src/ include/ -name "*.[ch]" | xargs clang-format --dry-run --Werror

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/share/ozhium-ollium
	install -m 0755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install -m 0755 $(UI_TARGET) $(DESTDIR)$(PREFIX)/bin/$(UI_TARGET)
	install -m 0644 example/ozhium-ollium.conf $(DESTDIR)$(PREFIX)/share/ozhium-ollium/
	install -m 0644 example/style.css $(DESTDIR)$(PREFIX)/share/ozhium-ollium/

compile_commands:
	@printf '[\n' > compile_commands.json
	@for src in $(DAEMON_SRC); do \
		obj=$$(echo "$$src" | sed 's|src/|out/|; s|\.c$$|.o|'); \
		cmd="gcc $(CFLAGS) -MMD -MP -c $$src -o $$obj"; \
		esc=$$(echo "$$cmd" | sed 's/"/\\"/g'); \
		printf '  {"directory":"$(CURDIR)","file":"%s","command":"%s","output":"%s"},\n' "$$src" "$$esc" "$$obj" >> compile_commands.json; \
	done
	@for src in $(filter-out src/shared/common.c src/shared/log.c,$(UI_GTK_SRC)); do \
		obj=$$(echo "$$src" | sed 's|src/|out/|; s|\.c$$|.o|'); \
		cmd="gcc $(CFLAGS) $(GTK_CFLAGS) -MMD -MP -c $$src -o $$obj"; \
		esc=$$(echo "$$cmd" | sed 's/"/\\"/g'); \
		printf '  {"directory":"$(CURDIR)","file":"%s","command":"%s","output":"%s"},\n' "$$src" "$$esc" "$$obj" >> compile_commands.json; \
	done
	@obj="out/third_party/inih/ini.o"; \
	cmd="gcc $(CFLAGS) -MMD -MP -c third_party/inih/ini.c -o $$obj"; \
	esc=$$(echo "$$cmd" | sed 's/"/\\"/g'); \
	printf '  {"directory":"$(CURDIR)","file":"third_party/inih/ini.c","command":"%s","output":"%s"},\n' "$$esc" "$$obj" >> compile_commands.json
	@for src in $(QT_UI_SRC); do \
		obj=$$(echo "$$src" | sed 's|src/|out/|; s|\.cpp$$|.o|'); \
		cmd="g++ $(CFLAGS) $(QT_CFLAGS) -MMD -MP -c $$src -o $$obj"; \
		esc=$$(echo "$$cmd" | sed 's/"/\\"/g'); \
		printf '  {"directory":"$(CURDIR)","file":"%s","command":"%s","output":"%s"},\n' "$$src" "$$esc" "$$obj" >> compile_commands.json; \
	done
	@sed -i '$$ s/,$$//' compile_commands.json
	@printf ']\n' >> compile_commands.json
