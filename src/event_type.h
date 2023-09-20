#ifndef EVENT_TYPE_H
#define EVENT_TYPE_H

#include "Arduino.h"

// Define an enumeration to represent system event types with hex values
typedef enum {
    EVENT_SYS_TYPE_START = 0x10,
    EVENT_SYS_TYPE_STOP = 0x12,
    EVENT_SYS_TYPE_PAUSE = 0x13,
    EVENT_SYS_TYPE_RESET = 0x14,
    // Add more system event types as needed with appropriate hex values
    // ...
} EVENT_TYPE_SYS;

// Define an enumeration to represent application applicative event types with hex values
typedef enum {
    EVENT_APP_TYPE_FOOT_PRESS = 0x40,
    EVENT_APP_TYPE_FOOT_RIGHT_PRESS = 0x41,
    EVENT_APP_TYPE_FOOT_LEFT_PRESS = 0x42,
    // Add more workout event types as needed with appropriate hex values
    // ...
} EVENT_TYPE_APP;


// Define a struct to represent an applicative event
typedef struct {
    EVENT_TYPE_APP type;               // Type of the event
    uint32_t timestamp;
} EVENT_APP;

// Define a struct to represent an applicative event
typedef struct {
    EVENT_TYPE_SYS type;               // Type of the event
    uint32_t timestamp;
} EVENT_SYS;

#endif