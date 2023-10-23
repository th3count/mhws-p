// gpio.h
#ifndef GPIO_H
#define GPIO_H

#include "data_structures.h"

// Function prototypes for GPIO control
void initialize_gpio(const struct GPIOSettings* settings, int numSettings);
void control_relay(int relayID, int state);
void manage_watering_cycle(int zoneID, int duration);

#endif
