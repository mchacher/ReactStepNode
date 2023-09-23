
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum  
{
    INIT,
    IDLE,
    RUNNING,                
    SETTING,             
    PAUSE,
} STATE_PRODUCT;

void state_machine_setup();
void state_machine_task();

#endif