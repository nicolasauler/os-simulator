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
void process_commands(WINDOW *console, char *buf);
int main_loop(WINDOW **wins, char *buf);

int main(void) {
    WINDOW *my_wins[4];
    PANEL *my_panels[4];
    PANEL_DATA panel_datas[4];
    PANEL_DATA *temp;
    int ch;
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

    /* update the stacking order. 2nd panel will be on top */
    update_panels();

    /* show it on the screen */
    attron(COLOR_PAIR(2));
    mvprintw(LINES - 2, 0, "Show or Hide a window with 'c'(Console)");
    mvprintw(LINES - 1, 0, "F1 to Exit");

    attroff(COLOR_PAIR(4));
    doupdate();

    temp = (PANEL_DATA *)panel_userptr(my_panels[3]);
    /*    while(main_loop_exit != 1) {
          while (temp->hide == FALSE) {
          }
          }
    */

    while ((ch = getch()) != KEY_F(1)) {
        main_loop_exit = main_loop(my_wins, command);
        if (main_loop_exit == 1) {
            break;
        }
        if ((ch == 9) || (main_loop_exit == 2)) {
            if (temp->hide == FALSE) {
                hide_panel(my_panels[3]);
                temp->hide = TRUE;
            } else {
                show_panel(my_panels[3]);
                temp->hide = FALSE;
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
    int startx, starty, height, width;

    getbegyx(win, starty, startx);
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
    temp = (width - length) / 2;
    x = startx + (int)temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    refresh();
}

/* move cursor to inside of the window, which represents a console line
 * then, make the panel behave like a prompt,
 * saving the user commmand to a buffer and returning it
 */
void process_commands(WINDOW *console, char *buf) {
    int ch;
    int x, y, i;

    getyx(console, y, x);
    x = 2;
    y += 2;
    /* print '$' character to indicate the prompt */
    wattron(console, COLOR_PAIR(4));
    mvwprintw(console, y, x, "$");
    wattroff(console, COLOR_PAIR(4));
    wmove(console, y, x + 2);
    wrefresh(console);
    i = 0;
    while ((ch = wgetch(console)) != KEY_F(1) && ch != 9 && ch != 10) {
        switch (ch) {
        case KEY_BACKSPACE:
        case 127:
        case 8:
            getyx(console, y, x);
            if (x > 1) {
                wmove(console, y, --x);
                waddch(console, ' ');
                wmove(console, y, x);
                buf[i--] = '\0';
            }
            break;
        default:
            getyx(console, y, x);
            waddch(console, ch);
            wmove(console, y, ++x);
            buf[i++] = ch;
            break;
        }
        wrefresh(console);
    }
    getyx(console, y, x);
    wmove(console, ++y, x = 4);
    buf[i] = '\0';
}

void show_commands(WINDOW *console) {
    int x, y;
    getyx(console, y, x);
    wmove(console, y, x = 2);
    waddstr(console, "create - create a new process\n");
    waddstr(console, "exit - exit the program\n");
    wrefresh(console);
}

/* while commands inputted by user are not 'create' or 'exit'
 * keep processing commands, and show the list of commands
 */
int main_loop(WINDOW **wins, char *buf) {
    while (strcmp(buf, "create") != 0 && strcmp(buf, "exit") != 0) {
        process_commands(wins[3], buf);
        if (strchr(buf, '\t') == NULL) {
            return 2;
        }
        if (strcmp(buf, "create") != 0 && strcmp(buf, "exit") != 0) {
            show_commands(wins[3]);
        }
    }
    if (strcmp(buf, "create") == 0) {
        return 0;
    } else {
        return 1;
    }
}
