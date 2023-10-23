// config.h
#ifndef CONFIG_H
#define CONFIG_H

#include "data_structures.h"

// Function prototypes for configuration
void parse_relays_config(struct Relay** relays, int* numRelays);
void parse_zones_config(struct Zone** zones, int* numZones);
void parse_timers_config(struct Timer** timers, int* numTimers);
void save_config_changes(const struct Relay* relays, int numRelays, const struct Zone* zones, int numZones, const struct Timer* timers, int numTimers);

#endif
