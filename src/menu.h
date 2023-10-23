// menu.h
#ifndef MENU_H
#define MENU_H

#include "data_structures.h"

enum MenuType {
    MAIN_MENU,
    PLANTS_MENU,
    TIMERS_MENU,
    ZONES_MENU,
    RELAYS_MENU
};

extern enum MenuType current_menu;

// Function prototypes for menu handling
void display_menu(enum MenuType menu);
void handle_user_input();

#endif
