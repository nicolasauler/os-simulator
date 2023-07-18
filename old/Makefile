# gcc -Wall -Wextra -Wshadow -ansi -pedantic -g -o main main.c
# depends on tui.c

CC=gcc
CFLAGS=-Wall -Wextra -Wshadow -ansi -pedantic -g
LDFLAGS=-lncurses
SOURCES=main.c tui.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
