#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "logger.h"
#include "scheduler.h"
#include <limits.h>
#include <ncurses.h>
#include <panel.h>
#include <string.h>

#define MAXSTR 20
#define MAXINSTS 20

typedef struct _PANEL_DATA {
    int hide; /* TRUE if panel is hidden */
} PANEL_DATA;

extern struct process processo;

void init_wins(WINDOW **wins);
void win_show(WINDOW *win, char *label, int label_color);
void print_in_middle(WINDOW *win, int starty, int startx, uint64_t width,
                     char *string, chtype color);
void show_commands(WINDOW *win);
void init_interface(WINDOW **my_wins, PANEL **my_panels,
                    PANEL_DATA *panel_datas);
void show_keyboard_shortcuts(void);
void print_prompt_char(WINDOW *console_win);
void do_backspace_action_on_console(WINDOW *win);
void do_enter_action_on_console(WINDOW *win);
void add_char_to_console(WINDOW *win, int ch);

void update_interface(WINDOW **wins, PANEL **panels, p_queue_t *p);

void restart_tcb(WINDOW *win);
void restart_status(WINDOW *win);
void restart_queue(WINDOW *win);
void restart_map(WINDOW *win);

void print_process_queue(WINDOW *win, p_queue_t *p);
void print_bit_map_of_processes_memory(WINDOW *win);
void read_instructions_file(WINDOW *win, p_queue_t *p);

void show_title(sched_info_t sched_info);

#endif
