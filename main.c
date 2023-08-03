/* operating systems simulator, with process management
 * and memory management
 * the user creates or kills processes
 * and the program automatically runs them and updates the interface
 */

#include "interface.h"
#include "logger.h"
#include "mmu.h"
#include "scheduler.h"
#include <pthread.h>
#include <unistd.h>

p_queue_t *p_queue = NULL;

typedef struct {
    WINDOW **wins;
    PANEL **panels;
    sched_info_t sched_info;
} thread_args;

/* char **commands is an array of chars that corresponds to user inputs
 * int n_strings is the number of strings in the array
 * the commands can be create -m <mem_size>, kill <pid> or just create
 * which creates a process with 1 memory
 * uses getopt to parse the command line arguments
 * if <mem_size> is bigger than 20, make user input a smaller number
 * */
void process_command(WINDOW **wins, char **commands, int n_strings,
                     p_queue_t **p, uint8_t *process_count);
char **tokenize_command(char *command, int *n_strings);
void *kernel(void *args);
sched_info_t parse_args(int argc, char **argv);

int main(int argc, char **argv) {
    WINDOW *my_wins[5];
    PANEL *my_panels[5];
    PANEL_DATA panel_datas[5];
    PANEL_DATA *temp;
    int ch;
    char command[MAXSTR];
    pthread_t kernel_thread;
    thread_args *args = malloc(sizeof(thread_args));
    int i = 0;
    uint8_t process_count = 0;
    sched_info_t sched_info = parse_args(argc, argv);

    init_mem();
    init_interface(my_wins, my_panels, panel_datas);
    show_keyboard_shortcuts();
    show_title(sched_info);
    log_init();

    args->wins = my_wins;
    args->panels = my_panels;
    args->sched_info = sched_info;
    pthread_create(&kernel_thread, NULL, kernel, args);

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
                    process_command(my_wins, commands, n_strings, &p_queue,
                                    &process_count);
                    update_interface(my_wins, my_panels, p_queue);
                }
                i = 0;
            } else {
                add_char_to_console(my_wins[3], ch);
                if (ch > CHAR_MAX) {
                    logger("invalid character, user inputted not ascii");
                    continue;
                } else {
                    command[i++] = (char)ch;
                }
            }
        }
        update_panels();
        doupdate();
    }

    endwin();
    free(args);
    pthread_cancel(kernel_thread);
    return (0);
}

void *kernel(void *args) {
    struct timespec tim = {0, 50000000L};

    WINDOW **my_wins = ((thread_args *)args)->wins;
    PANEL **my_panels = ((thread_args *)args)->panels;
    sched_info_t sched_info = ((thread_args *)args)->sched_info;

    while (1) {
        nanosleep(&tim, NULL);

        if (p_queue == NULL) {
            continue;
        }

        p_queue = run_process(p_queue, sched_info);
        update_interface(my_wins, my_panels, p_queue);
        sleep(1);
    }
}

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
                     p_queue_t **p, uint8_t *process_count) {
    if (strcmp(commands[0], "create") == 0) {
        int opt;
        process_t *p1;
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

        if (mem_size > MAXMEM) {
            show_commands(wins[3]);
        } else {
            p1 = create_process(mem_size, (*process_count));
            (*process_count) += 1;
            (*p) = add_process_to_queue((*p), p1);
            wprintw(wins[3], "create process with %d memory\n", mem_size);
        }
    } else if (strcmp(commands[0], "kill") == 0) {
        if (n_strings != 2) {
            show_commands(wins[3]);
        } else {
            int32_t pid = atoi(commands[1]);
            (*p) = kill_process(pid, (*p));
            wprintw(wins[3], "kill process %d\n", pid);
        }
    } else {
        show_commands(wins[3]);
    }

    free(commands);
}

/* programs receives as argument if the scheduler will be run in FIFO or RR mode
 * and what is the quantum, for the latter */
/* program will be run: ./main [FIFO|RR] -q [quantum], and the quantum is an
 * optional argument, defaulting to 2 */
/* parse using getopt */
sched_info_t parse_args(int argc, char **argv) {
    sched_info_t sched_info;
    int opt;

    sched_info.algorithm = FIFO;
    sched_info.time_quantum = 2;

    while ((opt = getopt(argc, argv, "q:")) != -1) {
        switch (opt) {
        case 'q':
            sched_info.algorithm = RR;
            sched_info.time_quantum = atoi(optarg);
            break;
        default:
            break;
        }
    }

    return sched_info;
}
