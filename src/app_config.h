#ifndef APP_CONFIG_H
#define APP_CONGIG_H


#define REACT_MESH              1 // set to 1 to activate RADIO
#define LOCAL_COMMAND_BUTTONS   1 // set to 1 to activate PLAY/PAUSE, STOP and CUSTOM Buttons
#define DIGITAL_FOOT_SENSOR     1 // set to 1 for digital foot sensor (typically on mock up)

#define TASK_CYCLE_FAST 20      // value in ms
#define TASK_CYCLE_MEDIUM 100
#define TASK_CYCLE_SLOW 250

#define PERIOD_HEARTBEAT (2000 / TASK_CYCLE_FAST) // every 2s, assuming 20ms as fast cycle (TODO, check what is the right timing)

#define MAX_NODES 100

#endif