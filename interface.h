#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "scheduler.h"
#include <ncurses.h>
#include <panel.h>
#include <string.h>

#define LINES 37
#define COLS 157
#define MAXSTR 20

typedef struct _PANEL_DATA {
    int hide; /* TRUE if panel is hidden */
} PANEL_DATA;

extern struct process processo;

void init_wins(WINDOW **wins, int n);
void win_show(WINDOW *win, char *label, int label_color);
void print_in_middle(WINDOW *win, int starty, int startx, int width,
                     char *string, chtype color);
void show_commands(WINDOW *win);
void init_interface(WINDOW **my_wins, PANEL **my_panels,
                    PANEL_DATA *panel_datas);
void show_keyboard_shortcuts(void);
void print_prompt_char(WINDOW *console_win);
void do_backspace_action_on_console(WINDOW *win);
void do_enter_action_on_console(WINDOW *win);
void add_char_to_console(WINDOW *win, int ch);

void update_interface(WINDOW **wins, p_circ_queue_t *p);

void print_in_queue(WINDOW *win, int starty, int startx, int width,
                    char *string);

void restart_status(WINDOW *win);
void restart_queue(WINDOW *win);
void restart_map(WINDOW *win);

void print_bit_map_of_processes_memory(WINDOW *win, p_circ_queue_t *p);
void read_instructions_file(WINDOW *win, p_circ_queue_t *p);

#endif
