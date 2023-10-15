
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum  
{
    INIT = 0,
    READY = 1,
    RUN = 2,                
    SET = 3,             
    PAUSE = 4
} STATE_PRODUCT;

void state_machine_setup();
void state_machine_task();
STATE_PRODUCT state_machine_get_active_state();

#endif