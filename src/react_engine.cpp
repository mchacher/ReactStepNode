
#include <Arduino.h>
#include <ArduinoLog.h>
#include "react_engine.h"
#include "led.h"
#include "event_type.h"
#include "event_registry.h"
#include "react_code.h"
#include "async_commands_list.h"

uint8_t exampleReactCode[] =
    {
        START,
        LED_COLOR, 0xFF, 0x00, 0x00, // LED ROUGE
        FOOT_PRESS_LEFT_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF,
        TIMER, 0, 20,                                                                      // Timer 2s
        LED_COLOR, (COLOR_BLUE >> 16) & 0xFF, (COLOR_BLUE >> 8) & 0xFF, COLOR_BLUE & 0xFF, // LED BLEUE
        WAIT_EVENT, EVENT_APP_TYPE_FOOT_PRESS_LEFT,                                        // WAIT EVENT FOOT PRESS
        END};

static int pc = 0;
static int last_pc = 0;
static uint16_t timer = 0;
static uint8_t waited_app_event = 0;
static unsigned long waited_app_event_time;
static RE_STATE re_state = RE_IDLE;
static uint16_t tick = 0;
static uint32_t foot_left_color = 0;

#define MAX_ASYNC_COMMANDS 10
AsyncCommandsList<MAX_ASYNC_COMMANDS> asyncCommandsList;

void foot_press_left_color()
{
    Log.noticeln(F("react_engine: foot_press_left_color"));
    led_set_color(foot_left_color);
}

void handle_application_events()
{
    EVENT app_event;
    if (event_registry_pop_app_event(app_event))
    {
        Log.noticeln(F("react_engine: handle_application_events"));
        ASYNC_COMMANDS *item = asyncCommandsList.getItemByEventType(app_event.type);
        if (item != nullptr)
        {
            item->handler_function();
        }
        else
        {
            Log.noticeln(F("react_engine: no async command identified for this event %i"), app_event.type);
        }
    }
}
// Function to handle the START command
void handleStartCommand() {
    // Handle START command
    Log.noticeln(F("react_engine: Command START"));
}

// Function to handle the END command
void handleEndCommand() {
    // Handle END command
    Log.noticeln(F("react_engine: Command END"));
    pc = 0;
}

// Function to handle the WAIT_EVENT command
void handleWaitEventCommand(uint8_t *bytecode) {
    re_state = RE_HOLD_WAIT_APP_EVENT;
    waited_app_event = bytecode[pc];
    waited_app_event_time = millis();
    pc = pc + 1;
    Log.noticeln(F("react_engine: Command WAIT_EVENT, event: %i"), waited_app_event);
    event_registry_enable_app_event();
}

// Function to handle the TIMER command
void handleTimerCommand(uint8_t *bytecode) {
    // Handle TIMER command with args
    Log.noticeln(F("react_engine: Command TIMER %l"), millis());
    timer = (uint16_t)(bytecode[pc] << 8);
    timer |= bytecode[pc + 1];
    pc = pc + 2;
    re_state = RE_HOLD_TIMER_ENABLE;
}

// Function to handle the LED_COLOR command
void handleLedColorCommand(uint8_t *bytecode) {
    Log.noticeln(F("react_engine: Command LED_COLOR"));
    uint32_t color = ((uint32_t)bytecode[pc] << 16);
    color |= ((uint32_t)bytecode[pc + 1] << 8);
    color |= bytecode[pc + 2];
    pc = pc + 3;
    led_set_color(color);
}

// Function to handle the FOOT_PRESS_LEFT_COLOR command
void handleFootPressLeftColorCommand(uint8_t *bytecode) {
    Log.notice(F("react_engine: Command FOOT_PRESS_LEFT_COLOR"));
    foot_left_color = ((uint32_t)bytecode[pc] << 16);
    foot_left_color |= ((uint32_t)bytecode[pc + 1] << 8);
    foot_left_color |= bytecode[pc + 2];
    pc = pc +3;
    // TODO check whether async_commands is not already registered
    ASYNC_COMMANDS ac;
    ac.bytecode = FOOT_PRESS_LEFT_COLOR;
    ac.active = true;
    ac.event_type = EVENT_APP_TYPE_FOOT_PRESS_LEFT;
    ac.handler_function = &foot_press_left_color;
    asyncCommandsList.add(ac);
    Log.noticeln(F("react_engine: adding handler for event %i"), ac.event_type);
}

// Function to interpret and execute commands with arguments
void interpret_command(uint8_t *bytecode) {
    uint8_t command = bytecode[pc];
    Log.noticeln(F("react_engine: command bytecode %i"), command);

    last_pc = pc;
    pc = pc + 1;
    switch (command) {
    case START:
        handleStartCommand();
        break;
    case END:
        handleEndCommand();
        break;
    case WAIT_EVENT:
        handleWaitEventCommand(bytecode);
        break;
    case TIMER:
        handleTimerCommand(bytecode);
        break;
    case SEND_EVENT:
        Log.noticeln(F("react_engine: Command SEND_EVENT NOT IMPLEMENTED"));
        // Handle SEND command
        break;
    case LED_COLOR:
        handleLedColorCommand(bytecode);
        break;
    case FOOT_PRESS_LEFT_COLOR:
        handleFootPressLeftColorCommand(bytecode);
        break;
    default:
        Log.noticeln(F("react_engine: Command UNKNOWN COMMAND"));
        // Unknown command
        break;
    }
}



void react_engine_setup()
{
    Log.notice(F("react_engine: setup" CR));
    re_state = RE_READY;
}

void react_engine_stop()
{
    re_state = RE_READY;
    pc = 0;
    waited_app_event = 0;
    event_registry_disable_app_event();
    led_set_color(COLOR_BLACK);
}

void react_engine_pause()
{
    re_state = RE_PAUSE;
    event_registry_disable_app_event();
}

void react_engine_task()
{
    switch (re_state)
    {
    case RE_READY:
        re_state = RE_RUN;
        event_registry_enable_app_event();
    case RE_RUN:
        tick = 0;
        interpret_command(exampleReactCode);
        handle_application_events();
        break;
    case RE_HOLD_TIMER_ENABLE:
        handle_application_events();
        tick++;
        if ((tick * REACT_ENGINE_CYCLE_TIME) == 1000) // 1s
        {
            timer = timer - 1;
            if (timer == 0)
            {
                re_state = RE_RUN;
            }
            tick = 0;
        }
        break;
    case RE_HOLD_WAIT_APP_EVENT:
        EVENT app_event;
        if (waited_app_event == EVENT_APP_TYPE_FOOT_PRESS)
        {
            if (event_registry_get_app_event(EVENT_APP_TYPE_FOOT_PRESS_LEFT, waited_app_event_time, app_event) || event_registry_get_app_event(EVENT_APP_TYPE_FOOT_PRESS_RIGHT, waited_app_event_time, app_event))
            {
                re_state = RE_RUN;
            }
        }
        else if (event_registry_get_app_event((EVENT_TYPE)waited_app_event, waited_app_event_time, app_event))
        {
            re_state = RE_RUN;
        }
        break;
    case RE_PAUSE:
        pc = last_pc;
        re_state = RE_READY;
    }
}