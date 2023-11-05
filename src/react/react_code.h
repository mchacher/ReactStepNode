#ifndef REACT_CODE_H
#define REACT_CODE_H

#include "event_type.h"

// Command codes
typedef enum {
    CMD_START = 0x80,
    CMD_END = 0x81,
    CMD_WAIT_EVENT = 0x82,
    CMD_TIMER_HOLD = 0x83,
    CMD_TIMER = 0x84,
    CMD_REPEAT_COUNT = 0x85,
    CMD_REPEAT_END = 0x86,
    CMD_LED_COLOR = 0xA0,
    CMD_LED_TRAFFIC_LIGHT = 0xA1,
    CMD_LED_EFFECT = 0xA2,
    CMD_FOOT_PRESS_COLOR = 0xB0,
    CMD_FOOT_PRESS_LEFT_COLOR = 0xB1,
    CMD_FOOT_PRESS_RIGHT_COLOR = 0xB2,
    CMD_FOOT_PRESS_COUNTER = 0xB3,
    CMD_FOOT_PRESS_COUNTER_RESET = 0xB4,
    CMD_SEND_EVENT = 0xc0
} COMMANDS;

typedef enum {
    TRUE = 0x20,
    FALSE = 0x21,
} ARGS;



#endif