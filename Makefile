CC      = gcc
CFLAGS  = -Wall -Wextra -I include
GTK_CFLAGS = $(shell pkg-config --cflags gtk4 gtk4-layer-shell-0)
GTK_LDFLAGS = $(shell pkg-config --libs gtk4 gtk4-layer-shell-0)

TARGET      = ozhium-ollium
UI_TARGET   = ozhium-ollium-ui

SRC_DIR = src
UI_SRC_DIR = src/ui
OBJ_DIR = out

.PHONY: all clean compile_commands.json

all: $(TARGET) $(UI_TARGET) compile_commands.json

$(TARGET): $(OBJ_DIR)/ozhium-ollium.o $(OBJ_DIR)/invoke.o $(OBJ_DIR)/tool-daemon.o
	$(CC) $^ -o $@ -lpulse

$(UI_TARGET): $(OBJ_DIR)/ui-main.o $(OBJ_DIR)/ui-config.o $(OBJ_DIR)/ui-window.o $(OBJ_DIR)/ui-builder.o $(OBJ_DIR)/ui-args.o $(OBJ_DIR)/tool-ui.o
	$(CC) $^ -o $@ $(GTK_LDFLAGS)

$(OBJ_DIR)/ozhium-ollium.o: $(SRC_DIR)/ozhium-ollium.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/invoke.o: $(SRC_DIR)/invoke.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/tool-daemon.o: $(SRC_DIR)/tool.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/tool-ui.o: $(SRC_DIR)/tool.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/ui-%.o: $(UI_SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

-include $(OBJ_DIR)/*.d

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(UI_TARGET)

compile_commands.json:
	@echo '[' > $@
	@echo '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) -c src/ozhium-ollium.c -o out/ozhium-ollium.o", "file": "src/ozhium-ollium.c"},' >> $@
	@echo '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) -c src/invoke.c -o out/invoke.o", "file": "src/invoke.c"},' >> $@
	@echo '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) -c src/tool.c -o out/tool-daemon.o", "file": "src/tool.c"},' >> $@
	@echo '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) -c src/tool.c -o out/tool-ui.o", "file": "src/tool.c"},' >> $@
	@echo '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) $(GTK_CFLAGS) -c src/ui/main.c -o out/ui-main.o", "file": "src/ui/main.c"},' >> $@
	@echo '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) $(GTK_CFLAGS) -c src/ui/config.c -o out/ui-config.o", "file": "src/ui/config.c"},' >> $@
	@echo '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) $(GTK_CFLAGS) -c src/ui/window.c -o out/ui-window.o", "file": "src/ui/window.c"},' >> $@
	@echo '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) $(GTK_CFLAGS) -c src/ui/builder.c -o out/ui-builder.o", "file": "src/ui/builder.c"},' >> $@
	@echo '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) $(GTK_CFLAGS) -c src/ui/args.c -o out/ui-args.o", "file": "src/ui/args.c"}' >> $@
	@echo ']' >> $@
