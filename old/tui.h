#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 30
#define HEIGHT 10

void print_menu(WINDOW *menu_win, int highlight);
void report_choice(int mouse_x, int mouse_y, int *p_choice);

extern char *choices[];
extern  int n_choices;
