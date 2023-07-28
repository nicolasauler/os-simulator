/* operating systems simulator, with process management
 * and memory management
 * the user creates or kills processes
 * and the program automatically runs them and updates the interface
 */

#include "interface.h"
#include "logger.h"
#include "scheduler.h"
#include <pthread.h>
#include <unistd.h>

p_circ_queue_t *p_queue = NULL;

typedef struct {
    WINDOW **wins;
    p_circ_queue_t *p;
} thread_args;

void process_command(WINDOW **wins, char **commands, int n_strings,
                     p_circ_queue_t **p, uint8_t *process_count);
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
    thread_args *args = malloc(sizeof(thread_args));
    int i = 0;
    uint8_t process_count = 0;

    init_interface(my_wins, my_panels, panel_datas);
    show_keyboard_shortcuts();

    args->wins = my_wins;
    args->p = p_queue;
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

                    update_interface(my_wins, p_queue);
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

    while (1) {
        nanosleep(&tim, NULL);

        if (p_queue == NULL) {
            continue;
        }

        p_queue = run_process(p_queue);
        sleep(5);
        update_interface(my_wins, p_queue);
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
                     p_circ_queue_t **p, uint8_t *process_count) {
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

        p1 = create_process(mem_size, (*process_count));
        (*process_count) += 1;
        (*p) = add_process_to_queue((*p), p1);
        wprintw(wins[3], "create process with %d memory\n", mem_size);
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
