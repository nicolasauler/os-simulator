#include "interface.h"
#include "mmu.h"
#include "scheduler.h"
#include <curses.h>
#include <panel.h>

extern struct _PANEL_DATA panel_data;

void init_wins(WINDOW **wins) {
    char label[80];
    int32_t max_x, max_y;

    getmaxyx(stdscr, max_y, max_x);

    wins[0] = newwin(5, max_x, 0, 0);
    sprintf(label, "Queue");
    win_show(wins[0], label, 1);

    wins[1] = newwin(max_y - 8, max_x / 3, 5, 0);
    sprintf(label, "Status");
    win_show(wins[1], label, 2);

    wins[4] = newwin(max_y - 8, max_x / 3, 5, max_x / 3);
    sprintf(label, "TCB");
    win_show(wins[4], label, 2);

    wins[2] = newwin(max_y - 8, max_x / 3, 5, max_x - (max_x / 3));
    sprintf(label, "Bit Map");
    win_show(wins[2], label, 2);

    wins[3] = newwin(max_y / 2, max_x / 2, (max_y / 2) + 1, max_x / 4);
    sprintf(label, "Console");
    scrollok(wins[3], TRUE);
    idlok(wins[3], TRUE);
    max_y = getmaxy(wins[3]);
    wsetscrreg(wins[3], 3, max_y - 1);
    win_show(wins[3], label, 3);
}

/* show the window with a border and a label */
void win_show(WINDOW *win, char *label, int label_color) {
    int32_t width;
    uint64_t width_conv;
    __attribute__((unused)) int height;

    getmaxyx(win, height, width);
    if (width < 0) {
        logger("ERROR: width < 0");
        exit(EXIT_FAILURE);
    } else {
        width_conv = (uint64_t)width;
    }

    box(win, 0, 0);
    mvwaddch(win, 2, 0, ACS_LTEE);
    mvwhline(win, 2, 1, ACS_HLINE, width - 2);
    mvwaddch(win, 2, width - 1, ACS_RTEE);

    print_in_middle(win, 1, 0, width_conv, label, COLOR_PAIR(label_color));
}

/* print a string in the middle of a window */
void print_in_middle(WINDOW *win, int starty, int startx, uint64_t width,
                     char *string, chtype color) {
    int x, y;
    uint64_t temp, length;
    int32_t tmp_conv;

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

    if (temp > INT32_MAX) {
        logger("ERROR: temp > MAX_INT32");
        exit(EXIT_FAILURE);
    } else {
        tmp_conv = (int32_t)temp;
    }

    x = startx + tmp_conv;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    wmove(win, y + 1, x);
    refresh();
}

void show_commands(WINDOW *console) {
    int x, y;
    getyx(console, y, x);
    wmove(console, y, x = 4);
    mvwaddstr(console, y, x, "create - create a new process\n");
    mvwaddstr(console, y + 1, x, "         create -m [mem] \n");
    mvwaddstr(console, y + 2, x, "         mem: 1 - 20\n");
    mvwaddstr(console, y + 3, x, "kill - kill a process\n");
    mvwaddstr(console, y + 4, x, "         delete [pid]\n");
    mvwaddstr(console, y + 5, x, "         pid: 1 - 64\n");
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
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);

    /* initialize windows */
    init_wins(my_wins);

    /* attach a panel to each window */
    my_panels[0] = new_panel(my_wins[0]);
    my_panels[1] = new_panel(my_wins[1]);
    my_panels[2] = new_panel(my_wins[2]);
    my_panels[4] = new_panel(my_wins[4]);
    my_panels[3] = new_panel(my_wins[3]);

    /* Initialize panel datas saying that nothing is hidden */
    panel_datas[0].hide = FALSE;
    panel_datas[1].hide = FALSE;
    panel_datas[2].hide = FALSE;
    panel_datas[4].hide = FALSE;
    panel_datas[3].hide = FALSE;

    set_panel_userptr(my_panels[0], &panel_datas[0]);
    set_panel_userptr(my_panels[1], &panel_datas[1]);
    set_panel_userptr(my_panels[2], &panel_datas[2]);
    set_panel_userptr(my_panels[4], &panel_datas[4]);
    set_panel_userptr(my_panels[3], &panel_datas[3]);

    update_panels();
}

void show_keyboard_shortcuts() {
    attron(COLOR_PAIR(4));
    mvprintw(LINES - 2, 0, "Show or Hide Console with 'Tab'");
    mvprintw(LINES - 1, 0, "F1 to Exit");
    attroff(COLOR_PAIR(4));
    doupdate();
}

void show_title(sched_info_t sched_info) {
    attron(COLOR_PAIR(6));
    mvprintw(LINES - 2, COLS - 28, "Process Management Simulator");
    if (sched_info.algorithm == FIFO) {
        mvprintw(LINES - 1, COLS - 10, "Using FIFO");
    } else {
        mvprintw(LINES - 1, COLS - 24, "Using RR with Quantum %d",
                 sched_info.time_quantum);
    }
    attroff(COLOR_PAIR(6));
    doupdate();
}

void print_prompt_char(WINDOW *console_win) {
    int x, y;
    getyx(console_win, y, x);
    x = 2;
    y += 1;
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

void reset_panels(PANEL **panels) {
    PANEL_DATA *temp;
    if ((temp = ((PANEL_DATA *)panel_userptr(panels[3])))->hide == FALSE) {
        hide_panel(panels[3]);
        temp->hide = TRUE;
        show_panel(panels[3]);
        temp->hide = FALSE;
    }
}

void print_tcb_of_current_process(WINDOW *win, p_queue_t *p) {
    p_queue_t *current = p;

    restart_tcb(win);

    if (current != NULL) {
        mvwprintw(win, 3, 1, "PID: %d\n", current->process->pid);
        mvwprintw(win, 4, 1, "PC: %d\n", 0);
        mvwprintw(win, 5, 1, "SP: %d\n", 0);
        mvwprintw(win, 6, 1, "Priority: %d\n", 0);
        mvwprintw(win, 7, 1, "State: %u\n", current->process->state);
    }
    wrefresh(win);
}

void update_interface(WINDOW **wins, PANEL **panels, p_queue_t *p) {
    print_process_queue(wins[0], p);
    print_bit_map_of_processes_memory(wins[2]);
    read_instructions_file(wins[1], p);
    print_tcb_of_current_process(wins[4], p);

    reset_panels(panels);
    update_panels();
    doupdate();
}

void print_process_queue(WINDOW *win, p_queue_t *p) {
    char pid_text[MAXSTR];
    p_queue_t *current = p;
    int helper = 0;

    restart_queue(win);
    wmove(win, 1, 1);

    if (current != NULL) {
        sprintf(pid_text, "PID: %d\n", current->process->pid);
        wattron(win, COLOR_PAIR(5));
        mvwprintw(win, 3, helper * 10 + 1, "%s", pid_text);
        wattroff(win, COLOR_PAIR(5));
        refresh();
        current = current->next;
    }
    while (current != NULL) {
        helper += 1;
        sprintf(pid_text, "PID: %d\n", current->process->pid);
        mvwprintw(win, 3, helper * 10 + 1, "%s", pid_text);
        refresh();
        current = current->next;
    }
}

void read_instructions_file(WINDOW *win, p_queue_t *p) {
    FILE *fp;
    char instructions[MAXINSTS][MAXSTR];
    int i = 0;
    int j = 0;
    int k = 0;
    p_queue_t *current = p;

    restart_status(win);

    if (p == NULL) {
        logger("No process in queue");
        return;
    }

    switch (current->process->pid % 5) {
    case 0:
        fp = fopen("insts/insts0.asm", "r");
        break;
    case 1:
        fp = fopen("insts/insts1.asm", "r");
        break;
    case 2:
        fp = fopen("insts/insts2.asm", "r");
        break;
    case 3:
        fp = fopen("insts/insts3.asm", "r");
        break;
    case 4:
        fp = fopen("insts/insts4.asm", "r");
        break;
    default:
        logger("Error opening file");
        exit(1);
    }

    if (fp == NULL) {
        logger("Error opening file");
        exit(1);
    }

    while (fgets(instructions[i], MAXSTR, fp) != NULL) {
        i += 1;
    }

    do {
        if (current->process->state == RUNNING) {
            k = current->process->time_used;
            break;
        }
        current = current->next;
    } while (current != NULL);

    current = p;
    if (current->process->state == RUNNING) {
        for (j = 0; j < i; j++) {
            instructions[j][strcspn(instructions[j], "\r\n")] = 0;
            if (j == k) {
                mvwprintw(win, j + 3, 3, "%s\t\t<----\n", instructions[j]);
            } else {
                mvwprintw(win, j + 3, 3, "%s\n", instructions[j]);
            }
        }
    }

    fclose(fp);
    wrefresh(win);
}

void restart_tcb(WINDOW *win) {
    char label[MAXSTR];
    werase(win);
    sprintf(label, "TCB");
    win_show(win, label, 2);
    wrefresh(win);
    doupdate();
}

void restart_status(WINDOW *win) {
    char label[MAXSTR];
    werase(win);
    sprintf(label, "Status");
    win_show(win, label, 2);
    wrefresh(win);
    doupdate();
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
    win_show(win, label, 2);
    wrefresh(win);
    doupdate();
}

void print_bit_map_of_processes_memory(WINDOW *win) {
    int i, j;
    int x, y;
    int32_t scalex, scaley;
    bool *memory = get_memory_map();

    restart_map(win);
    getmaxyx(win, y, x);
    scalex = (x - 6) / 6;
    scaley = (y - 6) / 6;
    wmove(win, 1, 1);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 5; j++) {
            mvwprintw(win, 2 + ((i + 1) * scaley), ((j + 0.75) * scalex), "%d",
                      memory[i * 5 + j]);
        }
    }
    refresh();
    free(memory);
}
