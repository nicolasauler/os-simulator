/* ncurses and panel program that has 2 windows for the user to
 * interact with as well as two panels/tabs
 * this program will be an operating system simulator
 * with a scheduler and memory manager
 */

#include "interface.h"
#include "scheduler.h"
#include <getopt.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <threads.h>
#include <unistd.h>

process process_list[MAXPRCS];

/*
typedef struct {
    WINDOW **wins;
    char **commands;
    int n_strings;
    process *p;
    uint8_t *process_count;
} thread_args; */

/*
 * commands are either create -m <int>, which creates a simulated
 * process, where the -m flag specifies size of memory used and is an
 * optional flag or kill <int>, which kills a simulated process use getopt
 * */
void process_command(WINDOW **wins, char **commands, int n_strings,
                     process p[MAXPRCS], uint8_t *process_count);

/* void *process_async_command(void *args); */
char **tokenize_command(char *command, int *n_strings);
void *kernel(void *args);

int main(void) {
    WINDOW *my_wins[4];
    PANEL *my_panels[4];
    PANEL_DATA panel_datas[4];
    PANEL_DATA *temp;
    int ch;
    char command[MAXSTR];
    pthread_t kernel_thread;
    int i = 0;
    uint8_t process_count = 0;

    init_interface(my_wins, my_panels, panel_datas);

    show_keyboard_shortcuts();

    /* launch the kernel as a separate pthread to continuously update the process
     * so it is updated non blocking in the interface */
    pthread_create(&kernel_thread, NULL, kernel, (void *)process_list);

    while (ch = getch(), ch != KEY_F(1)) {
        if ((temp = ((PANEL_DATA *)panel_userptr(my_panels[3])))->hide ==
                TRUE &&
            ch == 9) {
            show_panel(my_panels[3]);
            temp->hide = FALSE;
        } else if ((temp = ((PANEL_DATA *)panel_userptr(my_panels[3])))->hide ==
                   FALSE) {
            if (i == 0 && ch != 9 && ch != 8 && ch != KEY_BACKSPACE &&
                ch != 127) {
                print_prompt_char(my_wins[3]);
            }
            if (ch == 8 || ch == KEY_BACKSPACE || ch == 127) {
                do_backspace_action_on_console(my_wins[3]);
                if (i > 0) {
                    command[i--] = '\0';
                }
            } else if (ch == 9) {
                hide_panel(my_panels[3]);
                temp->hide = TRUE;
            } else if (ch == 10) {
                do_enter_action_on_console(my_wins[3]);
                command[i] = '\0';
                if (i > 0) {
                    int n_strings = 0;
                    char **commands = tokenize_command(command, &n_strings);
                    process_command(my_wins, commands, n_strings, process_list,
                                    &process_count);

                    /* thread_args *args = malloc(sizeof(thread_args));
                    args->wins = my_wins;
                    args->commands = commands;
                    args->n_strings = n_strings;
                    args->p = process_list;
                    args->process_count = &process_count;
                    pthread_t thread;
                    pthread_create(&thread, NULL, process_async_command, args);
                    pthread_join(thread, NULL);
                    free(args); */
                }
                i = 0;
            } else {
                add_char_to_console(my_wins[3], ch);
                command[i++] = ch;
            }
        }
        update_panels();
        doupdate();
    }

    endwin();
    pthread_cancel(kernel_thread);
    return (0);
}

void *kernel(void *args) {
    int i;
    int help;
    process *p = (process *)args;
    for (i = 0; i < MAXPRCS; i++) {
        if (p[i].state == RUNNING) {
            for (help = 0; help < p[i].mem_size; help++) {
                sleep(1);
            }
            p[i].state = TERMINATED;
            break;
        }
    }
    pthread_exit(NULL);
}

/*
void *process_async_command(void *args) {
    thread_args *t_args = (thread_args *)args;
    process_command(t_args->wins, t_args->commands, t_args->n_strings,
                    t_args->p, t_args->process_count);
    pthread_exit(NULL);
} */

char **tokenize_command(char *command, int *n_strings) {
    char *token = strtok(command, " ");
    char **commands = malloc(sizeof(char *) * MAXSTR);

    while (token != NULL) {
        commands[(*n_strings)++] = token;
        token = strtok(NULL, " ");
    }
    commands[(*n_strings)] = NULL;

    return commands;
}

void process_command(WINDOW **wins, char **commands, int n_strings,
                     process p[MAXPRCS], uint8_t *process_count) {
    if (strcmp(commands[0], "create") == 0) {
        int opt;
        int mem_size = 0;
        optind = 0;
        while ((opt = getopt(n_strings, commands, "m:")) != -1) {
            switch (opt) {
            case 'm':
                mem_size = atoi(optarg);
                break;
            default:
                break;
            }
        }
        if (mem_size == 0) {
            mem_size = 1;
        }
        create_process((*process_count), mem_size, p);
        (*process_count) += 1;
        update_interface(wins, p);
        wprintw(wins[3], "create process with %d memory\n", mem_size);
    } else if (strcmp(commands[0], "kill") == 0) {
        if (n_strings != 2) {
            show_commands(wins[3]);
        } else {
            int pid = atoi(commands[1]);
            kill_process(pid, p);
            update_interface(wins, p);
            wprintw(wins[3], "kill process %d\n", pid);
        }
    } else {
        show_commands(wins[3]);
    }

    free(commands);
}
