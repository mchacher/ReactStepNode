
#ifndef RUNTIME_H
#define RUNTIME_H


#define RUNTIME_CYCLE 10 // 10ms

enum RT_State 
{
    RT_RUN = 0,
    RT_TIMER_ENABLE = 1,
    RT_STOP = 2
};

void runtime_setup();
void runtime_task();

#endif