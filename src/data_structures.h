#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

// Define the data structures used in your project
struct Relay {
    int zoneID;
    int gpioPin;
    char alias[64];
    int state; // You can use an enumeration for HIGH/LOW
};

struct Zone {
    int ID;
    char plant[64];
    double emitterSize;
    double emitterRecommended;
    double emitterResponsibility;
    double waterRequirement;
    double waterActual;
};

struct Timer {
    int zoneID;
    char frequency[5]; // e.g., "2D" for twice daily
    int duration;
    char startTime[32]; // e.g., "08:00,16:00"
};

// You can add more data structures if needed

#endif
