#include <Arduino.h>
#include <ArduinoLog.h>
#include <TaskScheduler.h>
#include <printf.h>
#include "rf.h"
#include "state_machine.h"
#include "led.h"
#include "react_engine.h"
#include "event_registry.h"
#include "foot_sensor.h"
#include "display.h"
#include "button.h"
#include "hardware_config.h"

Scheduler runner;

#define TASK_CYCLE_FAST 20
#define TASK_CYCLE_MEDIUM 100
#define TASK_CYCLE_SLOW 250

// Tasks
Task task_led(TASK_CYCLE_MEDIUM, TASK_FOREVER, &led_task);
Task task_react_engine(REACT_ENGINE_CYCLE_TIME, TASK_FOREVER, &react_engine_task);
// extern Task task_react_engine;
Task task_event_registry(TASK_CYCLE_SLOW, TASK_FOREVER, &event_registry_task);
Task task_foot_sensor(5, TASK_FOREVER, &foot_sensor_task);
Task task_display(TASK_CYCLE_MEDIUM, TASK_FOREVER, &display_task);
Task task_button(TASK_CYCLE_FAST, TASK_FOREVER, &button_task);
Task task_state_machine(TASK_CYCLE_MEDIUM, TASK_FOREVER, &state_machine_task);

#include "Arduino.h"
#include "ArduinoLog.h"
#include "state_machine.h"
#include "event_type.h"
#include "event_registry.h"

static STATE_PRODUCT state;
unsigned long timestamp_last_state_transition = 0;

STATE_PRODUCT state_machine_get_active_state()
{
  return state;
}

void state_machine_switch_state(STATE_PRODUCT new_state)
{
  state = new_state;
  timestamp_last_state_transition = millis();
}

void state_machine_setup()
{
  state = INIT; // Initialize the state machine with the initial state
}

void state_machine_task()
{

  EVENT event;
  // Your code here to perform actions and transitions based on the current state
  // For example:
  switch (state)
  {
  case INIT:
    // Handle INIT state
    state_machine_switch_state(READY);
    Log.noticeln(F("state_machine_task: switching to READY state"));
    display_message(MSG_HELLO);
    break;

  case READY:

    // Handle IDLE state
    // need a START event to switch to RUNNING state
    if (event_registry_get_front_sys_event(event))
    {
      if (event.type == EVENT_SYS_TYPE_START)
      {
        state_machine_switch_state(RUN);
        Log.noticeln(F("state_machine_task: switching to RUN state"));
        task_react_engine.enable();
        event_registry_remove_front_sys_event();
        display_message(MSG_GO, 2);
      }
    }
    break;

  case RUN:
    // Handle RUNNING state
    // Transition to the next state if needed
    // display_message(MSG_GO);
    if (event_registry_get_front_sys_event(event))
    {
      switch (event.type)
      {
      case EVENT_SYS_TYPE_STOP:
        state_machine_switch_state(READY);
        Log.noticeln(F("state_machine_task: STOP event, switching to READY state"));
        task_react_engine.disable();
        react_engine_stop();
        event_registry_remove_front_sys_event();
        display_message(MSG_STOP);
        break;
      case EVENT_SYS_TYPE_START:
        state_machine_switch_state(READY);
        Log.noticeln(F("state_machine_task: PAUSE event, switching to READY state"));
        task_react_engine.disable();
        react_engine_pause();
        event_registry_remove_front_sys_event();
        display_message(MSG_PAUSE);
        break;
      }
    }
    break;

  case SET:
    // Handle SETTING state
    // Transition to the next state if needed
    state_machine_switch_state(PAUSE);
    break;

  case PAUSE:
    // Handle PAUSE state
    // Transition to the next state if needed
    state_machine_switch_state(READY);
    break;

  default:
    // Handle any unexpected or error state
    break;
  }

  // Your code here for any other tasks in the main loop
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }
  Log.begin(LOG_LEVEL_NOTICE, &Serial);
#ifdef MINI_STEP_MOCK_UP
  Log.notice(F("Main: Starting Mini Step Mockup with ID: %d" CR), NODE_ID);
#elif
  Log.notice(F("Main: Starting React Step Node with ID: %d" CR), NODE_ID);
#endif

  // setupRF();
  // initialize scheduler
  Log.notice(F("Main: initializing scheduler" CR));
  runner.init();
  Log.notice(F("Main: launching tasks" CR));

  // initialize state machine
  led_setup();
  runner.addTask(task_led);
  task_led.enable();

  // Create and Launch State Machine task
  state_machine_setup();
  runner.addTask(task_state_machine);
  task_state_machine.enable();

  // Create and Launch LED task
  led_setup();
  runner.addTask(task_led);
  task_led.enable();

  // Create and Launch Event Registry task
  event_registry_setup();
  runner.addTask(task_event_registry);
  task_event_registry.enable();

  // Create and Launch Foot Sensor task
  foot_sensor_setup();
  runner.addTask(task_foot_sensor);
  task_foot_sensor.enable();

  // Create and Launch Button task
  button_setup();
  runner.addTask(task_button);
  task_button.enable();

  // Create and Launch React Engine task
  react_engine_setup();
  runner.addTask(task_react_engine);
  // task_react_engine.enable();

  // Create and Launch Display task
  display_setup();
  runner.addTask(task_display);
  task_display.enable();
}

void loop()
{
  runner.execute();
}
