
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum  
{
    INIT,
    READY,
    RUN,                
    SET,             
    PAUSE,
} STATE_PRODUCT;

void state_machine_setup();
void state_machine_task();
STATE_PRODUCT state_machine_get_active_state();

#endif