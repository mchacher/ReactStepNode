#ifndef EVENT_TYPE_H
#define EVENT_TYPE_H

#include "Arduino.h"
#include "queue.h"

// Define an enumeration to represent system event types with hex values
typedef enum {
    // System Events
    EVENT_SYS_TYPE_START    = 0x10,
    EVENT_SYS_TYPE_STOP     = 0x12,
    EVENT_SYS_TYPE_PAUSE    = 0x13,
    EVENT_SYS_TYPE_SET      = 0x14,
    // Add more system event types as needed with appropriate hex values
    // ...
    EVENT_SYS_TYPE_RESET    = 0x3F,
    
    // Application Events
    EVENT_APP_TYPE_FOOT_PRESS           = 0x40,
    EVENT_APP_TYPE_FOOT_PRESS_LEFT      = 0x41,
    EVENT_APP_TYPE_FOOT_PRESS_RIGHT     = 0x42,
    EVENT_APP_TYPE_FOOT_RELEASE         = 0x43,
    EVENT_APP_TYPE_FOOT_RELEASE_LEFT    = 0x44,
    EVENT_APP_TYPE_FOOT_RELEASE_RIGHT   = 0x45,
    // Add more workout event types as needed with appropriate hex values
    // ...
} EVENT_TYPE;

// Define a struct to represent an applicative event
typedef struct {
    EVENT_TYPE type; // Type of the event
    uint32_t timestamp;
} EVENT;


#endif
