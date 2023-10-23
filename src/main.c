// main.c
#include <ncurses.h>
#include "menu.h"

enum MenuType current_menu = MAIN_MENU;

int main() {
    initscr();
    noecho();
    curs_set(FALSE);

    init_menus();

    while (1) {
        display_menu(current_menu);
        handle_user_input();
    }

    endwin();
    return 0;
}
