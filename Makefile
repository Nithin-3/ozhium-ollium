CC      = gcc
CFLAGS  = -Wall -Wextra -I include $(shell pkg-config --cflags gtk4 gtk4-layer-shell-0)
LDFLAGS = -lpulse $(shell pkg-config --libs gtk4 gtk4-layer-shell-0)
TARGET  = ozhium-ollium

SRC_DIR = src
OBJ_DIR = out

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

.PHONY: all clean compile_commands.json

all: $(TARGET) compile_commands.json

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

compile_commands.json: $(SRCS)
	@echo '[' > $@; \
	first=1; \
	for src in $(SRCS); do \
		obj=$(OBJ_DIR)/$$(basename $${src%.c}).o; \
		[ $$first -eq 0 ] && echo ',' >> $@; \
		printf '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) -c %s -o %s", "file": "%s"}' \
			"$$src" "$$obj" "$$src" >> $@; \
		first=0; \
	done; \
	echo ''; \
	echo ']' >> $@
