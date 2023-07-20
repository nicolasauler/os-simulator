/* ncurses and panel program that has 2 windows for the user to
 * interact with as well as two panels/tabs
 * this program will be an operating system simulator
 * with a scheduler and memory manager
 */

#include "interface.h"
#include "scheduler.h"
#include <getopt.h>
#include <stdint.h>
#include <stdlib.h>

process process_list[10];

int process_command(WINDOW **wins, PANEL **panels, char *command, process p[10],
                    uint8_t *process_count);

int main(void) {
    WINDOW *my_wins[4];
    PANEL *my_panels[4];
    PANEL_DATA panel_datas[4];
    PANEL_DATA *temp;
    int ch;
    char command[MAXSTR];
    int i = 0;
    int main_loop_exit = 0;
    uint8_t process_count = 0;

    init_interface(my_wins, my_panels, panel_datas);

    show_keyboard_shortcuts();

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
                    main_loop_exit =
                        process_command(my_wins, my_panels, command,
                                        process_list, &process_count);
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
    return (0);
}

int process_command(WINDOW **wins, PANEL **panels, char *command, process p[10],
                    uint8_t *process_count) {
    int j = 0;
    char *token = strtok(command, " ");
    char **commands = malloc(sizeof(char *) * MAXSTR);

    while (token != NULL) {
        commands[j++] = token;
        token = strtok(NULL, " ");
    }
    commands[j] = NULL;

    /* process user commands
     * commands are either create -m <int>, which creates a simulated
     * process, where the -m flag specifies size of memory used and is an
     * optional flag or kill <int>, which kills a simulated process use
     * getopt. also, if user enters create -m, only accept if also has inputted
     * the <int> argument to the -m option
     * */
    if (strcmp(commands[0], "create") == 0) {
        int opt;
        int mem_size = 0;
        optind = 0;
        while ((opt = getopt(j, commands, "m")) != -1) {
            switch (opt) {
            case 'm':
                mem_size = atoi(optarg);
                break;
            default:
                break;
            }
        }
        /*
        if (mem_size == 0) {
            mem_size = 1;
        } */
        create_process((*process_count), mem_size, p);
        (*process_count) += 1;
        update_interface(wins, panels, p);
        wprintw(wins[3], "create process with %d memory\n", mem_size);
    } else if (strcmp(commands[0], "kill") == 0) {
        int pid = atoi(commands[1]);
        wprintw(wins[3], "kill process %d\n", pid);
    } else if (strcmp(commands[0], "help") == 0) {
        show_commands(wins[3]);
    } else if (strcmp(commands[0], "exit") == 0) {
        free(commands);
        return (1);
    } else {
        show_commands(wins[3]);
    }

    free(commands);
    return (0);
}
