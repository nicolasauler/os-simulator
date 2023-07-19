/* ncurses and panel program that has 2 windows for the user to
 * interact with as well as two panels/tabs
 * this program will be an operating system simulator
 * with a scheduler and memory manager
 */

#include "interface.h"
#include <stdlib.h>

int process_command(WINDOW *win, int cmd_size, char *command);

int main(void) {
    WINDOW *my_wins[4];
    PANEL *my_panels[4];
    PANEL_DATA panel_datas[4];
    PANEL_DATA *temp;
    int ch, i;
    char command[MAXSTR];
    int main_loop_exit = 0;

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
                main_loop_exit = process_command(my_wins[3], i, command);
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

int process_command(WINDOW *win, int cmd_size, char *command) {
    if (cmd_size > 0) {
        if (strcmp(command, "create") == 0) {
            /* wprintw(my_wins[1], "create"); */
        } else if (strcmp(command, "delete") == 0) {
        } else if (strcmp(command, "exit") == 0) {
            return(1);
        } else {
            show_commands(win);
        }
    }
    return(0);
}
