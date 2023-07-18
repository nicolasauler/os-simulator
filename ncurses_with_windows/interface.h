#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define MAXSTR 10
#define LINES 37
#define COLS 157

void init_curses();
void console(WINDOW *winA, WINDOW *winB, WINDOW *winC, WINDOW *winD);
void create_layout(WINDOW **winA, WINDOW **winB, WINDOW **winC, WINDOW **winD);
