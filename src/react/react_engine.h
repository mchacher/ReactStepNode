
#ifndef REACT_ENGINE_H
#define REACT_ENGINE_H

#define REACT_ENGINE_CYCLE_TIME 10 // 10ms

#include "event_type.h"
typedef enum  
{
    RE_IDLE,                    // No program to run
    RE_READY,                   // Program is loaded, ready to execute
    RE_STARTING,                // Program is getting ready for execution
    RE_RUN,                     // Program is under-execution with no timer running
    RE_RUN_TIMER_ENABLE,        // Program is running with a timer countdown activated
    RE_PAUSE,                   // Program is paused
    RE_HOLD_TIMER_ENABLE,       // Program is on-hold, with a timer countdown activated
    RE_HOLD_WAIT_APP_EVENT,     // Program is on-hold, waiting for a timer event
    RE_DONE                     // Program has been successfully executed
} RE_STATE;

typedef struct {
    int pc;
    uint16_t timer;
    RE_STATE state;
    uint16_t foot_press_counter;
    unsigned long waited_app_event_time;
    EVENT waited_app_event;
    bool timer_display = false;
} Context;

typedef struct  {
    int pc;
    uint16_t count;
    bool display;
} RepeatContext;


void react_engine_setup();
void react_engine_task();
void react_engine_stop();
void react_engine_pause();
void react_engine_save_react_code(uint8_t* code);

#endif