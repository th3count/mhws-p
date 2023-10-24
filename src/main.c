// main.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "gpio.h"
#include "menu.h"
#include "timer.h"

int main() {
    // Initialize GPIO settings and load configuration
    initialize_gpio();
    load_config();

    while (1) {
        // Check the timer for each zone
        for (int i = 0; i < numTimers; i++) {
            // ... (previous timer logic remains the same)
        }

        // Update the menu and handle user input
        display_menu(current_menu);
        handle_user_input();

        // Add a delay to control the frequency of checking the timers
        usleep(1000000); // Sleep for 1 second
    }

    // Clean up and exit
    cleanup_gpio();
    save_config();

    return 0;
}
