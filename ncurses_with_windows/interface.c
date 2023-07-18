#include "interface.h"
#include <getopt.h>

#define MAXARGS 10

/* parses the user command, using getopt */
void parse_command(char *command) {
    int c;
    char *token;
    char *args[MAXARGS];
    int i = 0;

    token = strtok(command, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

/* displays it in a panel that can be hidden */
    while ((c = getopt(i, args, "h")) != -1) {
        switch (c) {
            case 'h':
                printf("Usage: %s [options]\n", args[0]);
                printf("Options:\n");
                printf("  -h\t\t\tShow this help message\n");
                break;
            default:
                printf("Usage: %s [options]\n", args[0]);
                printf("Try '%s -h' for more information.\n", args[0]);
                break;
        }
    }
}

char *read_file(char *filename) {
    long length;
    char *buffer = 0;
    FILE *f = fopen(filename, "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length);
        if (buffer) {
            fread(buffer, 1, length, f);
        }
        fclose(f);
    }
    return buffer;
}

void print_status(int current_isntr, int current_counter, WINDOW *winA,
                  char *instr) {
    wmove(winA, 2 + current_counter, 1);
    wclrtoeol(winA);
    waddstr(winA, instr);
    if (current_isntr == current_counter) {
        wmove(winA, 2 + current_counter, 20);
        waddstr(winA, " <----");
    }
    wrefresh(winA);
}

void console(WINDOW *winA, WINDOW *winB, WINDOW *winC, WINDOW *winD) {
    char command[MAXSTR];
    int i;
    char *buffer = read_file("process.asm");

    /* move cursor to window C */
    wmove(winC, 2, 1);
    wrefresh(winC);

    /* get a string from the user */
    echo();
    wgetnstr(winC, command, MAXSTR);
    noecho();

    /* parse the user command */
    parse_command(command);

    if (buffer) {
        char *pch;
        pch = strtok(buffer, "\n");
        for (i = 0; pch != NULL; i++) {
            print_status(2, i, winA, pch);
            pch = strtok(NULL, "\n");
        }
    }
}

void create_layout(WINDOW **winA, WINDOW **winB, WINDOW **winC, WINDOW **winD) {
    *winD = newwin(5, COLS, 0, 0);
    box(*winD, '|', '-');
    mvwaddstr(*winD, 1, 1, "Queue");
    wrefresh(*winD);

    *winA = newwin(LINES - 9, COLS / 2, 5, 0);
    box(*winA, '|', '-');
    mvwaddstr(*winA, 1, 1, "Status");
    wrefresh(*winA);

    *winB = newwin(LINES - 9, COLS / 2, 5, 79);
    box(*winB, '|', '-');
    mvwaddstr(*winB, 1, 1, "TCB");
    wrefresh(*winB);

    *winC = newwin(4, COLS, LINES - 4, 0);
    box(*winC, '|', '-');
    mvwaddstr(*winC, 1, 1, "Console");
    wrefresh(*winC);
}

void init_curses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    /*    curs_set(0); */
}
