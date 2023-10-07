/**
 * @file main.cpp
 * @brief Main program file for the React Step Node application.
 */
#include <Arduino.h>
#include <ArduinoLog.h>
#include "hardware_config.h"
#include <printf.h>
#include "state_machine.h"
#include "hmi/led.h"
#include "reactmagic/react_engine.h"
#include "reactmagic/event_registry.h"
#include "hmi/foot_sensor.h"
#include "hmi/display.h"
#include "hmi/button.h"
#include "react_scheduler.h"
#include "drivers/mic.h"
#if REACT_MESH == 1
#include "com/rf.h"
#endif

#define TASK_CYCLE_FAST 20
#define TASK_CYCLE_MEDIUM 100
#define TASK_CYCLE_SLOW 250

ReactScheduler runner;

#if REACT_MESH == 1
communication::rf comm;
uint32_t counter(0);

void taskCommReceiveFunction() {
  comm.receive();
}
void taskCommSendFunction() {
  comm.send(0, counter);
  counter += 1;
}
#endif

// Tasks
Task task_led(TASK_CYCLE_FAST, &led_task);
Task task_react_engine(REACT_ENGINE_CYCLE_TIME, &react_engine_task);
Task task_event_registry(TASK_CYCLE_SLOW, &event_registry_task);
Task task_foot_sensor(TASK_CYCLE_FAST, &foot_sensor_task);
Task task_display(TASK_CYCLE_MEDIUM, &display_task);
Task task_button(TASK_CYCLE_FAST, &button_task);
Task task_state_machine(TASK_CYCLE_MEDIUM, &state_machine_task);
#if REACT_MESH == 1
Task taskCommReceive(TASK_CYCLE_FAST, &taskCommReceiveFunction);
Task taskCommSend(2000, &taskCommSendFunction);
// Be careful, the receive task for the master shall work fastly (at 1s it isn't work)
#endif
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
    Log.noticeln(F("state_machine_task: switching to RUN state" CR));
    led_set_color(COLOR_BLACK);
    task_react_engine.enable();
    display_message(MSG_GO, 2);
    break;
  case EVENT_SYS_TYPE_SET_LP:
    state_machine_switch_state(SET);
    Log.noticeln(F("state_machine_task: SET event, switching to SET state" CR));
#if REACT_MESH == 1
    display_number(comm.getNodeId());
#endif
    display_blink_numbers(true);
    break;
  default:
    break;
  }
}

void handle_pause_state(EVENT event)
{
  switch (event.type)
  {
  case EVENT_SYS_TYPE_START:
    state_machine_switch_state(RUN);
    Log.noticeln(F("state_machine_task: switching to RUN state" CR));
    task_react_engine.enable();
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
    Log.noticeln(F("state_machine_task: STOP event, switching to READY state" CR));
    task_react_engine.disable();
    react_engine_stop();
    display_message(MSG_STOP);
    led_set_effect(LED_EFFECTS::EFFECT_RAINBOW);
    break;

  case EVENT_SYS_TYPE_START:
    state_machine_switch_state(PAUSE);
    Log.noticeln(F("state_machine_task: PAUSE event, switching to PAUSE state" CR));
    task_react_engine.disable();
    react_engine_pause();
    display_message(MSG_PAUSE);
    // led_set_effect(LED_EFFECTS::EFFECT_RAINBOW);
    break;

  case EVENT_SYS_TYPE_PAUSE:
    break;

  default:
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
    Log.verboseln(F("state_machine_task: SET event, switching to READY state" CR));
    display_blink_numbers(false);
    state_machine_switch_state(READY);
    display_message(MSG_IDLE);
    led_set_effect(LED_EFFECTS::EFFECT_RAINBOW);
    break;
  case EVENT_SYS_TYPE_SET_SP:
    Log.verboseln(F("state_machine_task: SET_SP event" CR));
#if REACT_MESH == 1
    comm.incrementNode_id();
    display_number(comm.getNodeId());
#endif
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
      handle_pause_state(event);
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
    delay(10);

  Log.begin(LOG_LEVEL_NOTICE, &Serial);

  Log.noticeln(F("[%s] Starting UP"), __func__);

#if REACT_MESH == 1
  // ONLY FOR ADRI testing (without react step proto)
  randomSeed(analogRead(0));
  uint8_t rand(random(256));
  for (int i = 0; i <= rand; i++)
  {
    comm.incrementNode_id();
  }

  Log.noticeln(F("[%s] Starting React Step Node with ID: [%d]"), __func__, comm.getNodeId());
  if (0 == comm.getNodeId())
  {
    Log.noticeln(F(" ---------- MASTER ----------"));
  }
  else
  {
    Log.noticeln(F("---------- NODE %d ----------"), comm.getNodeId());
  }

  // Init communication module
  comm.setup();
#else
  Log.noticeln(F("Starting React Step Node - REACT MESH is not activated!"));
#endif
  // Initialize Tasks
  Log.noticeln(F("Initialized scheduler"));

#if defined(MINI_STEP_MOCK_UP)
  Log.notice(F("Main: Starting Mini Step Mockup"));
#elif defined(MICRO_STEP_MOCK_UP)
  Log.noticeln(F("Main: Starting Micro Step Mockup"));
#endif

  // initialize scheduler
  Log.noticeln(F("Main: initializing scheduler"));
  runner.init();
  Log.notice(F("Main: launching tasks"));

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
  // Create and Launch Button task
  button_setup();
  runner.addTask(task_button);
  task_button.enable();

  // Create and Launch React Engine task
  react_engine_setup();
  runner.addTask(task_react_engine);

  // Create and Launch Display task
  display_setup();
  runner.addTask(task_display);
  task_display.enable();

#if REACT_MESH == 1
  runner.addTask(taskCommReceive);
  taskCommReceive.enable();
  Log.noticeln(F("--- taskCommReceive: added and enabled"));
  runner.addTask(taskCommSend);
  taskCommSend.enable();
  Log.noticeln(F("--- taskCommSend: added and enabled"));
#endif

  setup_mic();
  event_registry_push(EVENT_SYS_TYPE_READY);
}

void loop()
{
  runner.execute();
}
