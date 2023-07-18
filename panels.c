/* generate a ncurses and panel program that has 2 windows for the user to
 * interact with as well as two panels/tabs
 */

#include <curses.h>
#include <ncurses.h>
#include <panel.h>
#include <stdlib.h>
#include <string.h>

#define LINES 37
#define COLS 157
#define MAXSTR 20

typedef struct _PANEL_DATA {
    int hide; /* TRUE if panel is hidden */
} PANEL_DATA;

void init_wins(WINDOW **wins, int n);
void win_show(WINDOW *win, char *label, int label_color);
void print_in_middle(WINDOW *win, int starty, int startx, int width,
                     char *string, chtype color);
void show_commands(WINDOW *win);

int main(void) {
    WINDOW *my_wins[4];
    PANEL *my_panels[4];
    PANEL_DATA panel_datas[4];
    PANEL_DATA *temp;
    int ch, x, y, i;
    char command[MAXSTR];
    int main_loop_exit = 0;

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

    /* show it on the screen */
    attron(COLOR_PAIR(4));
    mvprintw(LINES - 2, 0, "Show or Hide a window with 'Tab'(Console)");
    mvprintw(LINES - 1, 0, "F1 to Exit");
    attroff(COLOR_PAIR(4));
    doupdate();

    while (main_loop_exit != 1) {
        ch = getch();
        if (ch == KEY_F(1)) {
            main_loop_exit = 1;
            break;
        } else if ((temp = ((PANEL_DATA *)panel_userptr(my_panels[3])))->hide ==
                       TRUE &&
                   ch == 9) {
            show_panel(my_panels[3]);
            temp->hide = FALSE;
        } else if ((temp = ((PANEL_DATA *)panel_userptr(my_panels[3])))->hide ==
                   FALSE) {
            if (i == 0 && ch != 9 && ch != 8 && ch != KEY_BACKSPACE &&
                ch != 127) {
                getyx(my_wins[3], y, x);
                x = 2;
                y += 2;
                /* print '$' character to indicate the prompt */
                wattron(my_wins[3], COLOR_PAIR(4));
                mvwprintw(my_wins[3], y, x, "$");
                wattroff(my_wins[3], COLOR_PAIR(4));
                wmove(my_wins[3], y, x + 2);
                wrefresh(my_wins[3]);
            }
            if (ch == 8 || ch == KEY_BACKSPACE || ch == 127) {
                getyx(my_wins[3], y, x);
                if (x > 4) {
                    wmove(my_wins[3], y, x - 1);
                    /* waddch(console, ' ');
                    wmove(console, y, x); */
                    wdelch(my_wins[3]);
                    if (i > 0) {
                        command[i--] = '\0';
                    }
                }
            } else if (ch == 9) {
                hide_panel(my_panels[3]);
                temp->hide = TRUE;
            } else if (ch == 10) {
                getyx(my_wins[3], y, x);
                wmove(my_wins[3], ++y, x = 4);
                command[i] = '\0';
                /* process command after enter */
                if (i > 0) {
                    if (strcmp(command, "create") == 0) {
                        /* wprintw(my_wins[1], "create"); */
                    } else if (strcmp(command, "delete") == 0) {
                    } else if (strcmp(command, "exit") == 0) {
                        main_loop_exit = 1;
                    } else {
                        show_commands(my_wins[3]);
                    }
                }
                i = 0;
            } else {
                getyx(my_wins[3], y, x);
                waddch(my_wins[3], ch);
                wmove(my_wins[3], y, x + 1);
                command[i++] = ch;
            }
        }
        update_panels();
        doupdate();
    }

    endwin();
    return (0);
}

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
