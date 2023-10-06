#ifndef DISPLAY_H
#define DISPLAY_H

#include "Arduino.h"

const PROGMEM char MSG_NONE[] = "";
const PROGMEM char MSG_HELLO[] = "HELLO";
const PROGMEM char MSG_READY[] = "READY";
const PROGMEM char MSG_STOP[] = "STOP";
const PROGMEM char MSG_PAUSE[] = "PAUSE";
const PROGMEM char MSG_GO[] = " GO ";
const PROGMEM char MSG_ERROR[] = "ERROR";
const PROGMEM char MSG_SET[] = " SET";
const PROGMEM char MSG_IDLE[] = "----";

struct QueuedMessage
{
    const char* msg;
    uint8_t duration;
};

void display_setup();
void display_task();
void display_message(const char *message);
void display_message(const char *message, uint8_t duration);
void display_number(uint16_t num);
void display_clear();
void display_push_message_to_queue(const char *message, uint8_t duration);
void display_blink_numbers(bool state);

#endif
