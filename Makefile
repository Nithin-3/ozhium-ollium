CC = gcc
CFLAGS = -Wall -Wextra -I include
LDFLAGS = -lpulse
TARGET = ozhium-ollium

SRCS = src/ozhium-ollium.c src/invoke.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean compile_commands.json

all: $(TARGET) compile_commands.json

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

compile_commands.json:
	@echo '[{"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) -c src/ozhium-ollium.c -o src/ozhium-ollium.o", "file": "src/ozhium-ollium.c"}]' > $@
