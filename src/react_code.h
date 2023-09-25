#ifndef REACT_CODE_H
#define REACT_CODE_H

#include "event_type.h"

// Command codes
enum {
    START = 0x80,
    END = 0x81,
    WAIT_EVENT = 0x82,
    TIMER = 0x83,
    SEND_EVENT = 0x84,
    LED_COLOR = 0xA0,
    LED_TRAFFIC_LIGHT = 0xA1,
    FOOT_PRESS_COLOR = 0xB0,
    FOOT_PRESS_LEFT_COLOR = 0xB1,
    FOOT_PRESS_RIGHT_COLOR = 0xB2
};

// Argument codes
#define COLOR_WHITE 0xFFFFFF
#define COLOR_BLACK 0x000000
#define COLOR_RED 0xFF0000
#define COLOR_GREEN 0x008000
#define COLOR_BLUE 0x0000FF
#define COLOR_PINK 0xFFC0CB
#define COLOR_ORANGE 0xFFA500
#define COLOR_YELLOW 0xFFFF00

typedef struct 
{
    bool active;
    uint8_t bytecode;
    EVENT_TYPE event_type;
    void (*handler_function)();
} ASYNC_COMMANDS;

#endif