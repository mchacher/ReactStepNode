#ifndef BUTTON_H
#define BUTTON_H
#if LOCAL_COMMAND_BUTTONS == 1
#include "Arduino.h"

void button_setup();
void button_task();

typedef enum {
    BUTTON_PRESS,
    BUTTON_LONG_PRESS,
    BUTTON_NO_EVENT
} ButtonEvent;
#endif
#endif // BUTTON_H
