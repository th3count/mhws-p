// menu.c

#include <ncurses.h>
#include "menu.h"

enum MenuType current_menu = MAIN_MENU;

void display_menu(enum MenuType menu) {
    clear(); // Clear the screen

    // Display menu choices horizontally at the top of the screen
    mvprintw(0, 0, "Main Menu   1. Timers   2. Zones   3. Relays   4. Test   5. Quit");

    // Display a table with relay information below the menu
    mvprintw(2, 0, "Zone ID  Alias          State");
    mvprintw(3, 0, "--------------------------------");
    // You can loop through and print relay information here
    // For example, display relay information in rows from 4 onward
    // e.g., mvprintw(4, 0, "1        Front Yard     ON");
    // You'll need to format the relay information as needed

    // Refresh the screen to display the menu and relay information
    refresh();
}

void handle_user_input() {
    int choice = getch(); // Get user input

    // Handle user input based on the current menu
    switch (current_menu) {
        case MAIN_MENU:
            switch (choice) {
                case '1':
                    current_menu = TIMERS_MENU;
                    break;
                case '2':
                    current_menu = ZONES_MENU;
                    break;
                case '3':
                    current_menu = RELAYS_MENU;
                    break;
                case '4':
                    // Add logic to enable all relays for 5 seconds and then disable them
                    test_relays();
                    break;
                case '5':
                    // Add logic to exit the program
                    break;
                default:
                    // Handle invalid input
                    break;
            }
            break;

        // Add cases to handle user input for other menu types (Timers, Zones, Relays) as needed
    }
}
