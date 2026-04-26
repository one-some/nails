TARGET = nails
CC     = gcc
CFLAGS = -g -Wall -lraylib -lm -fsanitize=address -Isrc
LDFLAGS = $(CFLAGS)

.PHONY: default all clean run
default: $(TARGET)
all: default

SOURCES = $(shell find src -name '*.c' -not -path './build/*')
HEADERS = $(shell find src -name '*.h' -not -path './build/*')

OBJECTS = $(patsubst src/%, build/%, $(SOURCES:.c=.o))

build/%.o: src/%.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@

clean:
	-rm -rf build
	-rm -f $(TARGET)

run: $(TARGET)
	-./$(TARGET)
