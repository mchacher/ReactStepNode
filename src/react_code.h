#ifndef REACT_CODE_H
#define REACT_CODE_H

#include "event_type.h"

// Command codes
typedef enum {
    START = 0x80,
    END = 0x81,
    WAIT_EVENT = 0x82,
    TIMER = 0x83,
    SEND_EVENT = 0x84,
    LED_COLOR = 0xA0,
    LED_TRAFFIC_LIGHT = 0xA1,
    LED_EFFECT = 0xA2,
    FOOT_PRESS_COLOR = 0xB0,
    FOOT_PRESS_LEFT_COLOR = 0xB1,
    FOOT_PRESS_RIGHT_COLOR = 0xB2,
    FOOT_PRESS_COUNTER = 0xB3,
    FOOT_PRESS_COUNTER_RESET = 0xB4
} COMMANDS;

typedef enum {
    TRUE = 0x20,
    FALSE = 0x21,
} ARGS;



#endif