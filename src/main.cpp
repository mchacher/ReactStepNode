/**
 * @file main.cpp
 * @brief Main program file for the React Step Node application.
 */

#include <Arduino.h>
#include <ArduinoLog.h>
#include <printf.h>
#include "rf.h"
#include "state_machine.h"
#include "led.h"
#include "react_engine.h"
#include "event_registry.h"
#include "foot_sensor.h"
#include "display.h"
#if defined(MINI_REACT_STEP_MOCK_UP)
#include "button.h"
#endif
#include "hardware_config.h"
#include "react_scheduler.h"
#include "mic.h"

ReactScheduler runner;

#define TASK_CYCLE_FAST 20
#define TASK_CYCLE_MEDIUM 100
#define TASK_CYCLE_SLOW 250

// Tasks
Task task_led(TASK_CYCLE_FAST, &led_task);
Task task_rf(TASK_CYCLE_FAST, &rf_task);
Task task_react_engine(REACT_ENGINE_CYCLE_TIME, &react_engine_task);
Task task_event_registry(TASK_CYCLE_SLOW, &event_registry_task);
Task task_foot_sensor(TASK_CYCLE_FAST, &foot_sensor_task);
Task task_display(TASK_CYCLE_MEDIUM, &display_task);
#if defined(MINI_REACT_STEP_MOCK_UP)
  Task task_button(TASK_CYCLE_FAST, &button_task);
#endif
Task task_state_machine(TASK_CYCLE_MEDIUM, &state_machine_task);

static STATE_PRODUCT state;
unsigned long timestamp_last_state_transition = 0;

/**
 * @brief Get the currently active state of the state machine.
 * @return The current state.
 */
STATE_PRODUCT state_machine_get_active_state()
{
  return state;
}

/**
 * @brief Switch the state of the state machine to a new state.
 * @param new_state The new state to switch to.
 */
void state_machine_switch_state(STATE_PRODUCT new_state)
{
  state = new_state;
  timestamp_last_state_transition = millis();
}

/**
 * @brief Initialize the state machine with the initial state.
 */
void state_machine_setup()
{
  state = INIT;
}

/**
 * @brief Handle the INIT state.
 * @param event The event to handle.
 */
void handle_init_state(EVENT event)
{
  if (event.type == EVENT_SYS_TYPE_READY)
  {
    state_machine_switch_state(READY);
    Log.noticeln(F("state_machine_task: READY event, switching to READY state"));
    display_message(MSG_HELLO, 2);
    display_push_message_to_queue(MSG_IDLE, 0);
    led_set_effect(LED_EFFECTS::EFFECT_RAINBOW);
  }
}

/**
 * @brief Handle the READY state.
 * @param event The event to handle.
 */
void handle_ready_state(EVENT event)
{
  switch (event.type)
  {
  case EVENT_SYS_TYPE_START:
    state_machine_switch_state(RUN);
    Log.noticeln(F("state_machine_task: switching to RUN state"));
    task_react_engine.enable();
    display_message(MSG_GO, 2);
    led_set_color(COLOR_BLACK);
    break;
  case EVENT_SYS_TYPE_SET_LP:
    state_machine_switch_state(SET);
    Log.noticeln(F("state_machine_task: SET event, switching to SET state"));
    // display_number(rf_get_node_id());
    display_blink_numbers(true);
    break;
  default:
    break;
  }
}

/**
 * @brief Handle the RUN state.
 * @param event The event to handle.
 */
void handle_run_state(EVENT event)
{
  // Handle RUNNING state
  switch (event.type)
  {
  case EVENT_SYS_TYPE_STOP:
    state_machine_switch_state(READY);
    Log.noticeln(F("state_machine_task: STOP event, switching to READY state"));
    task_react_engine.disable();
    react_engine_stop();
    display_message(MSG_STOP);
    led_set_effect(LED_EFFECTS::EFFECT_RAINBOW);
    break;
  case EVENT_SYS_TYPE_START:
    state_machine_switch_state(READY);
    Log.noticeln(F("state_machine_task: PAUSE event, switching to READY state"));
    task_react_engine.disable();
    react_engine_pause();
    display_message(MSG_PAUSE);
    led_set_effect(LED_EFFECTS::EFFECT_RAINBOW);
    break;
  }
}

/**
 * @brief Handle the SET state.
 * @param event The event to handle.
 */
void handle_set_state(EVENT event)
{
  switch (event.type)
  {
  case EVENT_SYS_TYPE_SET_LP:
    Log.verboseln(F("state_machine_task: SET event, switching to READY state"));
    display_blink_numbers(false);
    state_machine_switch_state(READY);
    display_message(MSG_IDLE);
    led_set_effect(LED_EFFECTS::EFFECT_RAINBOW);
    break;
  case EVENT_SYS_TYPE_SET_SP:
    Log.verboseln(F("state_machine_task: SET_SP event"));
    // rf_increment_node_id();
    // display_number(rf_get_node_id());
  default:
    break;
  }
}

/**
 * @brief Main state machine task that processes events and transitions between states.
 */
void state_machine_task()
{
  EVENT event;
  if (event_registry_pop_sys_event(event))
  {
    switch (state)
    {
    case INIT:
      handle_init_state(event);
      break;
    case READY:
      handle_ready_state(event);
      break;
    case RUN:
      handle_run_state(event);
      break;
    case SET:
      handle_set_state(event);
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
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }
  Log.begin(LOG_LEVEL_NOTICE, &Serial);
#if defined(MINI_STEP_MOCK_UP)
  Log.notice(F("Main: Starting Mini Step Mockup"));
#elif defined(MICRO_STEP_MOCK_UP)
  Log.noticeln(F("Main: Starting Micro Step Mockup"));
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

  // Create and Launch Event Registry task
  event_registry_setup();
  runner.addTask(task_event_registry);
  task_event_registry.enable();

  // Create and Launch Foot Sensor task
  foot_sensor_setup();
  runner.addTask(task_foot_sensor);
  task_foot_sensor.enable();
#if defined(MINI_REACT_STEP_MOCK_UP)
  // Create and Launch Button task
  button_setup();
  runner.addTask(task_button);
  task_button.enable();
#endif

  // Create and Launch React Engine task
  react_engine_setup();
  runner.addTask(task_react_engine);

  // Create and Launch Display task
  display_setup();
  runner.addTask(task_display);
  task_display.enable();

  rf_setup();
  runner.addTask(task_rf);
  task_rf.enable();

  setup_mic();

  event_registry_push(EVENT_SYS_TYPE_READY);
}

void loop()
{
  runner.execute();
}
