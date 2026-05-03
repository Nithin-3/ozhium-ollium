CC      = gcc
CFLAGS  = -Wall -Wextra -I include
GTK_CFLAGS = $(shell pkg-config --cflags gtk4 gtk4-layer-shell-0)
GTK_LDFLAGS = $(shell pkg-config --libs gtk4 gtk4-layer-shell-0)

TARGET      = ozhium-ollium
UI_TARGET   = ozhium-ollium-ui

OBJ_DIR = out

DAEMON_SRC = src/daemon/ozhium-ollium.c src/daemon/invoke.c src/daemon/battery.c src/daemon/pulse.c src/daemon/tool.c src/daemon/backLight.c
UI_SRC = src/ui/main.c src/ui/config.c src/ui/window.c src/ui/builder.c src/ui/args.c src/ui/tool.c

.PHONY: all clean

all: $(TARGET) $(UI_TARGET)

$(TARGET): $(OBJ_DIR)/daemon/ozhium-ollium.o $(OBJ_DIR)/daemon/invoke.o $(OBJ_DIR)/daemon/battery.o $(OBJ_DIR)/daemon/pulse.o $(OBJ_DIR)/daemon/tool.o $(OBJ_DIR)/daemon/backLight.o
	$(CC) $^ -o $@ -lpulse -ludev

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

$(OBJ_DIR)/ui/%.o: src/ui/%.c | $(OBJ_DIR)/ui
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/daemon:
	mkdir -p $@

$(OBJ_DIR)/ui:
	mkdir -p $@

-include $(OBJ_DIR)/**/*.d

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(UI_TARGET)
