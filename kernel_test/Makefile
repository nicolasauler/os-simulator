CC=gcc
CFLAGS=-Wall -Wextra -Wshadow -std=c99 -pedantic -g
#LDFLAGS=
SOURCES=kernel.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=kernel

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(EXECUTABLE)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
