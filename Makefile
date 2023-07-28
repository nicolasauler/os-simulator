CC=gcc
# add nanosleep with posix source
CFLAGS=-Wall -Wextra -Wshadow -Wfloat-equal -Wundef -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wconversion -ansi -pedantic -g -D_POSIX_C_SOURCE=199309L
LDFLAGS=-lncurses -ltinfo -lpanel -lpthread
SOURCES=main.c interface.c scheduler.c logger.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(EXECUTABLE)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
