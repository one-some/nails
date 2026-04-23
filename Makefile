TARGET = nails
CC = gcc
CFLAGS = -g -Wall -lraylib -fsanitize=address
LDFLAGS = $(CFLAGS)

.PHONY: default all clean run

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@

clean:
	-rm *.o
	-rm $(TARGET)

run: $(TARGET)
	-./$(TARGET)

