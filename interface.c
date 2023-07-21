#include "interface.h"

extern struct _PANEL_DATA panel_data;

/* Put all the windows */
void init_wins(WINDOW **wins, int n) {
    int i;
    char label[80];

    wins[0] = newwin(5, COLS, 0, 0);
    sprintf(label, "Queue");
    win_show(wins[0], label, 1);
    wins[1] = newwin(LINES - 8, COLS / 2, 5, 0);
    sprintf(label, "Status");
    win_show(wins[1], label, 2);
    wins[2] = newwin(LINES - 8, COLS / 2, 5, 79);
    sprintf(label, "Bit Map");
    win_show(wins[2], label, 3);
    for (i = 3; i < n; ++i) {
        wins[i] = newwin(LINES - 10, COLS - 10, 6, 5);
        sprintf(label, "Console");
        win_show(wins[i], label, i + 1);
    }
}

/* show the window with a border and a label */
void win_show(WINDOW *win, char *label, int label_color) {
    int width;
    __attribute__((unused)) int height;

    getmaxyx(win, height, width);

    box(win, 0, 0);
    mvwaddch(win, 2, 0, ACS_LTEE);
    mvwhline(win, 2, 1, ACS_HLINE, width - 2);
    mvwaddch(win, 2, width - 1, ACS_RTEE);

    print_in_middle(win, 1, 0, width, label, COLOR_PAIR(label_color));
}

/* print a string in the middle of a window */
void print_in_middle(WINDOW *win, int starty, int startx, int width,
                     char *string, chtype color) {
    int length, x, y;
    float temp;

    if (win == NULL) {
        win = stdscr;
    }
    getyx(win, y, x);
    if (startx != 0) {
        x = startx;
    }
    if (starty != 0) {
        y = starty;
    }
    if (width == 0) {
        width = 80;
    }

    length = strlen(string);
    temp = (float)(width - length) / 2;
    x = startx + (int)temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    refresh();
}

void show_commands(WINDOW *console) {
    int x, y;
    getyx(console, y, x);
    wmove(console, y, x = 4);
    mvwaddstr(console, y, x, "create - create a new process\n");
    mvwaddstr(console, y + 1, x, "         create [priority] [time]\n");
    mvwaddstr(console, y + 2, x, "         priority: 0 - 3\n");
    mvwaddstr(console, y + 3, x, "         time: 1 - 10\n");
    mvwaddstr(console, y + 4, x, "delete - delete a process\n");
    mvwaddstr(console, y + 5, x, "         delete [pid]\n");
    mvwaddstr(console, y + 6, x, "         pid: 1 - 10\n");
    mvwaddstr(console, y + 7, x, "exit - exit the program\n");
    wrefresh(console);
}

/* initializes the interface with all the other helper functions */
void init_interface(WINDOW **my_wins, PANEL **my_panels,
                    struct _PANEL_DATA *panel_datas) {
    /* initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    /* initialize all the colors */
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_BLACK, COLOR_WHITE);

    /* initialize windows */
    init_wins(my_wins, 4);

    /* attach a panel to each window */
    my_panels[0] = new_panel(my_wins[0]); /* push 0, order: stdscr-0 */
    my_panels[1] = new_panel(my_wins[1]); /* push 1, order: stdscr-0-1 */
    my_panels[2] = new_panel(my_wins[2]); /* push 2, order: stdscr-0-1-2 */
    my_panels[3] = new_panel(my_wins[3]); /* push 3, order: stdscr-0-1-2-3 */

    /* Initialize panel datas saying that nothing is hidden */
    panel_datas[0].hide = FALSE;
    panel_datas[1].hide = FALSE;
    panel_datas[2].hide = FALSE;
    panel_datas[3].hide = FALSE;

    set_panel_userptr(my_panels[0], &panel_datas[0]);
    set_panel_userptr(my_panels[1], &panel_datas[1]);
    set_panel_userptr(my_panels[2], &panel_datas[2]);
    set_panel_userptr(my_panels[3], &panel_datas[3]);

    update_panels();
}

void show_keyboard_shortcuts() {
    attron(COLOR_PAIR(4));
    mvprintw(LINES - 2, 0, "Show or Hide a window with 'Tab'(Console)");
    mvprintw(LINES - 1, 0, "F1 to Exit");
    attroff(COLOR_PAIR(4));
    doupdate();
}

void print_prompt_char(WINDOW *console_win) {
    int x, y;
    getyx(console_win, y, x);
    x = 2;
    y += 2;
    wattron(console_win, COLOR_PAIR(4));
    mvwprintw(console_win, y, x, "$");
    wattroff(console_win, COLOR_PAIR(4));
    wmove(console_win, y, x + 2);
    wrefresh(console_win);
}

void do_backspace_action_on_console(WINDOW *win) {
    int x, y;
    getyx(win, y, x);
    if (x > 4) {
        wmove(win, y, x - 1);
        /* waddch(console, ' ');
        wmove(console, y, x); */
        wdelch(win);
    }
}

void do_enter_action_on_console(WINDOW *win) {
    int x, y;
    getyx(win, y, x);
    wmove(win, ++y, x = 4);
}

void add_char_to_console(WINDOW *win, int ch) {
    int x, y;
    getyx(win, y, x);
    waddch(win, ch);
    wmove(win, y, x + 1);
}

/* print process p to whe window 1 */
void update_interface(WINDOW **wins, process p[MAXPRCS]) {
    int i;
    int n_actives = 0;
    int helper = 0;
    char pid_text[MAXSTR];

    restart_queue(wins[0]);
    wmove(wins[0], 1, 1);

    for (i = 0; i < MAXPRCS; i++) {
        if (p[i].is_active == 1) {
            n_actives += 1;
        }
    }
    for (i = 0; i < MAXPRCS; i++) {
        if (p[i].is_active == 1) {
            sprintf(pid_text, "PID: %d\n", p[i].pid);
            /* invert the colors before printing */
            if (helper == 0) {
                wattron(wins[0], COLOR_PAIR(5));
            }
            mvwprintw(wins[0], 3, helper * 10 + 1, "%s", pid_text);
            if (helper == 0) {
                wattroff(wins[0], COLOR_PAIR(5));
            }
            refresh();

            if (helper < n_actives) {
                helper += 1;
            }
        }
    }

    /* print a matrix of 6 by 6 zeros in win[2] */
    print_bit_map_of_processes_memory(wins[2], p);

    doupdate();
}

void print_in_queue(WINDOW *win, int starty, int startx, int width,
                    char *string) {
    int length, x, y;
    float temp;

    if (win == NULL) {
        win = stdscr;
    }
    getyx(win, y, x);
    if (startx != 0) {
        x = startx;
    }
    if (starty != 0) {
        y = starty;
    }
    if (width == 0) {
        width = 80;
    }

    length = strlen(string);
    temp = (float)(width - length) / 2;
    x = startx + (int)temp;
    mvwprintw(win, y, x, "%s", string);
    refresh();
}

void restart_queue(WINDOW *win) {
    char label[MAXSTR];
    werase(win);
    sprintf(label, "Queue");
    win_show(win, label, 1);
    wrefresh(win);
    doupdate();
}

void restart_map(WINDOW *win) {
    char label[MAXSTR];
    werase(win);
    sprintf(label, "Bit Map");
    win_show(win, label, 3);
    wrefresh(win);
    doupdate();
}

void print_bit_map_of_processes_memory(WINDOW *win, process *p) {
    int i, j;
    int x, y;
    float scalex, scaley;
    restart_map(win);
    getmaxyx(win, y, x);
    scalex = (float)(x - 6) / 6;
    scaley = (float)(y - 6) / 6;
    wmove(win, 1, 1);
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 6; j++) {
            /* print zeros to populate a map throughout the window */
            mvwprintw(win, 2 + ((i + 1) * scaley), ((j + 0.75) * scalex), "%d", 0);
        }
    }
    refresh();
}
