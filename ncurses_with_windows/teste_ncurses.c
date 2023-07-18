/* generate a ncurses program that has 2 windows for the user to interact with
 */

#include "interface.h"

int main(void) {
    WINDOW *winA, *winB, *winC, *winD;

    /* initialize ncurses */
    init_curses();

    /* create the windows */
    create_layout(&winA, &winB, &winC, &winD);

    while (1) {
        console(winA, winB, winC, winD);
    }

    /* wait for user input */
    wmove(winA, 2, 1);
    wclrtoeol(winA);
    waddstr(winA, "Press any key to continue");
    wrefresh(winA);
    getch();

    delwin(winA);
    delwin(winB);
    delwin(winC);

    endwin();
    return (0);
}
