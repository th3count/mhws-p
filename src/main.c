#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <wiringPi.h>

// Define the maximum number of relay channels
#define MAX_RELAY_CHANNELS 8

// Structure to hold relay information
typedef struct {
    int zoneID;
    int gpioPin;
    char alias[64];
    int state; // 0 for LOW, 1 for HIGH
} Relay;

// Structure to hold timer information
typedef struct {
    int zoneID;
    char frequency[32];
    int duration;
    char startTime[16];
} Timer;

// Function to parse the relays.xml file
void parseRelaysXML(const char *xmlFileName, Relay relays[], int *relayCount) {
    xmlDoc *doc = NULL;
    xmlNode *root = NULL;
    xmlNode *relayNode = NULL;

    doc = xmlReadFile(xmlFileName, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", xmlFileName);
        exit(EXIT_FAILURE);
    }

    root = xmlDocGetRootElement(doc);

    *relayCount = 0;

    for (relayNode = root->children; relayNode; relayNode = relayNode->next) {
        if (relayNode->type == XML_ELEMENT_NODE && xmlStrcmp(relayNode->name, (const xmlChar *)"relay") == 0) {
            xmlNode *node = relayNode->children;
            Relay relay;

            while (node != NULL) {
                if (node->type == XML_ELEMENT_NODE) {
                    if (xmlStrcmp(node->name, (const xmlChar *)"zone_ID") == 0) {
                        relay.zoneID = atoi(xmlNodeGetContent(node));
                    } else if (xmlStrcmp(node->name, (const xmlChar *)"gpioPin") == 0) {
                        relay.gpioPin = atoi(xmlNodeGetContent(node));
                    } else if (xmlStrcmp(node->name, (const xmlChar *)"alias") == 0) {
                        strncpy(relay.alias, (char *)xmlNodeGetContent(node), sizeof(relay.alias));
                    } else if (xmlStrcmp(node->name, (const xmlChar *)"state") == 0) {
                        relay.state = (strcmp((char *)xmlNodeGetContent(node), "HIGH") == 0) ? 1 : 0;
                    }
                }
                node = node->next;
            }

            relays[*relayCount] = relay;
            (*relayCount)++;
        }
    }

    xmlFreeDoc(doc);
}

// Function to parse the timers.xml file
void parseTimersXML(const char *xmlFileName, Timer timers[], int *timerCount) {
    xmlDoc *doc = NULL;
    xmlNode *root = NULL;
    xmlNode *timerNode = NULL;

    doc = xmlReadFile(xmlFileName, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", xmlFileName);
        exit(EXIT_FAILURE);
    }

    root = xmlDocGetRootElement(doc);

    *timerCount = 0;

    for (timerNode = root->children; timerNode; timerNode = timerNode->next) {
        if (timerNode->type == XML_ELEMENT_NODE && xmlStrcmp(timerNode->name, (const xmlChar *)"timer") == 0) {
            xmlNode *node = timerNode->children;
            Timer timer;

            while (node != NULL) {
                if (node->type == XML_ELEMENT_NODE) {
                    if (xmlStrcmp(node->name, (const xmlChar *)"zone_ID") == 0) {
                        timer.zoneID = atoi(xmlNodeGetContent(node));
                    } else if (xmlStrcmp(node->name, (const xmlChar *)"frequency") == 0) {
                        strncpy(timer.frequency, (char *)xmlNodeGetContent(node), sizeof(timer.frequency));
                    } else if (xmlStrcmp(node->name, (const xmlChar *)"duration") == 0) {
                        timer.duration = atoi(xmlNodeGetContent(node));
                    } else if (xmlStrcmp(node->name, (const xmlChar *)"startTime") == 0) {
                        strncpy(timer.startTime, (char *)xmlNodeGetContent(node), sizeof(timer.startTime));
                    }
                }
                node = node->next;
            }

            timers[*timerCount] = timer;
            (*timerCount)++;
        }
    }

    xmlFreeDoc(doc);
}

// Function to control relays based on the watering schedule and relay configuration
void controlRelays(const Timer timers[], int timerCount, const Relay relays[], int relayCount) {
    // Initialize WiringPi
    wiringPiSetup();

    while (1) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);

        for (int i = 0; i < timerCount; i++) {
            // Find the corresponding relay for this zone ID
            int relayIndex = -1;
            for (int j = 0; j < relayCount; j++) {
                if (relays[j].zoneID == timers[i].zoneID) {
                    relayIndex = j;
                    break;
                }
            }

            if (relayIndex != -1) {
                // Decode the frequency code and determine if watering should occur today
                int shouldWater = 0;
                char *freqCode = timers[i].frequency;
                char *token = strtok(freqCode, "-");

                if (token != NULL) {
                    if (strcmp(token, "D") == 0) {
                        // Daily watering
                        shouldWater = 1;
                    } else if (strcmp(token, "W") == 0) {
                        // Specific days of the week
                        token = strtok(NULL, "-");
                        while (token != NULL) {
                            char day = token[0];
                            if (day == 'M' && tm_info->tm_wday == 1) shouldWater = 1;
                            if (day == 'T' && tm_info->tm_wday == 2) shouldWater = 1;
                            if (day == 'W' && tm_info->tm_wday == 3) shouldWater = 1;
                            if (day == 'T' && tm_info->tm_wday == 4) shouldWater = 1;
                            if (day == 'F' && tm_info->tm_wday == 5) shouldWater = 1;
                            if (day == 'S' && tm_info->tm_wday == 6) shouldWater = 1;
                            if (day == 'S' && tm_info->tm_wday == 0) shouldWater = 1;
                            token = strtok(NULL, ",");
                        }
                    } else if (strcmp(token, "M") == 0) {
                        // Specific days of the month
                        token = strtok(NULL, "-");
                        while (token != NULL) {
                            int day = atoi(token);
                            if (day == tm_info->tm_mday) shouldWater = 1;
                            token = strtok(NULL, ",");
                        }
                    }
                }

                if (shouldWater) {
                    // Check if it's the correct time to water today
                    char *startTime = timers[i].startTime;
                    if (strcmp(startTime, "08:00") == 0 &&
                        tm_info->tm_hour == 8 && tm_info->tm_min == 0) {
                        // Control the relay based on its state in relays.xml
                        digitalWrite(relays[relayIndex].gpioPin, relays[relayIndex].state);
                        delay(timers[i].duration * 60 * 1000);
                        // Ensure the relay is off when watering is complete
                        digitalWrite(relays[relayIndex].gpioPin, !relays[relayIndex].state);
                    }
                }
            }
        }

        // Check relay states and handle mid-watering initialization
        for (int i = 0; i < relayCount; i++) {
            int relayState = digitalRead(relays[i].gpioPin);

            if (relayState == relays[i].state) {
                // Handle mid-watering initialization
                // Implement logic based on relay state and time if needed
            }
        }

        // Delay for a minute before checking again
        sleep(60);
    }
}

int main() {
    const char *timersXMLFile = "config/timers.xml";
    const char *relaysXMLFile = "config/relays.xml";

    Timer timers[MAX_RELAY_CHANNELS];
    Relay relays[MAX_RELAY_CHANNELS];

    int timerCount = 0;
    int relayCount = 0;

    // Parse timers.xml
    parseTimersXML(timersXMLFile, timers, &timerCount);

    // Parse relays.xml
    parseRelaysXML(relaysXMLFile, relays, &relayCount);

    // Control relays and monitor relay states
    controlRelays(timers, timerCount, relays, relayCount);

    return 0;
}
