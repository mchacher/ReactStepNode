
#ifndef RUNTIME_H
#define RUNTIME_H


#define RUNTIME_CYCLE 10 // 10ms

enum RT_State 
{
    RT_IDLE,                // No program to run
    RT_READY,               // Program is loaded, ready to execute
    RT_STARTING,            // Program is getting ready for execution
    RT_RUN,                 // Program is under-execution
    RT_PAUSE,               // Program is paused
    RT_HOLD_TIMER_ENABLE,    // Program is on-hold, with a timer countdown running
    RT_HOLD_WAIT_APP_EVENT   // Program is on-hold, waiting for a timer event
};

void runtime_setup();
void runtime_task();

#endif