#include <Arduino.h>
#include <ArduinoLog.h>
#include "react_engine.h"
#include "led.h"
#include "event_type.h"
#include "event_registry.h"
#include "react_code.h"
#include "async_commands_list.h"
#include "display.h"

uint8_t exampleReactCode[] =
    {
        START,
        LED_COLOR, 0xFF, 0x00, 0x00, // LED ROUGE
        TIMER, 0, 5, ARGS::TRUE,
        FOOT_PRESS_LEFT_COLOR, (COLOR_PINK >> 16) & 0xFF, (COLOR_PINK >> 8) & 0xFF, COLOR_PINK & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
        FOOT_PRESS_RIGHT_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
        FOOT_PRESS_COUNTER_RESET,
        FOOT_PRESS_COUNTER, ARGS::FALSE,
        TIMER, 0, 5, ARGS::FALSE,
        FOOT_PRESS_LEFT_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
        FOOT_PRESS_RIGHT_COLOR, (COLOR_PINK >> 16) & 0xFF, (COLOR_PINK >> 8) & 0xFF, COLOR_PINK & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
        FOOT_PRESS_COUNTER, ARGS::TRUE,
        TIMER, 0, 5, ARGS::FALSE,                                                                      // Timer 2s
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
static uint16_t foot_press_counter = 0;
static bool timer_display = false;

#define MAX_ASYNC_COMMANDS 10
AsyncCommandsList<MAX_ASYNC_COMMANDS> asyncCommandsList;

/**
 * @brief Change the color of the LED based on parameters.
 *
 * @param params An array of two color values [0] for pressed and [1] for released states.
 */
void change_react_device_color(uint32_t *params)
{
    Log.verbose(F("react_engine: change_react_device_color"));
    led_set_color(params[0]);
}

/**
 * @brief Change the color of the LED based on parameters.
 *
 * @param params An array of two color values [0] for pressed and [1] for released states.
 */
void update_react_device_foot_press_counter(uint32_t *params)
{
    Log.verbose(F("react_engine: update_foot_press_counter"));
    foot_press_counter++;
    if (params[0])
    {
        display_number(foot_press_counter);
    }
}

/**
 * @brief Handle application events by checking the event registry and executing corresponding commands.
 */
void handle_application_events()
{
    EVENT app_event;
    if (event_registry_pop_app_event(app_event))
    {
        Log.noticeln(F("react_engine: handle_application_events"));
        const size_t MAX_RESULTS = 10;
        ASYNC_COMMANDS *resultItems[MAX_RESULTS];
        size_t itemCount = asyncCommandsList.getItems(app_event.type, resultItems, MAX_RESULTS);
        for (int i = 0; i < itemCount; i++)
        {
            resultItems[i]->handler_function(resultItems[i]->params);
        }
    }
}

/**
 * @brief Handle the START command.
 */
void handleStartCommand()
{
    // Handle START command
    Log.noticeln(F("---------------------------"));
    Log.noticeln(F("react_engine: Command START"));
}

/**
 * @brief Handle the END command.
 */
void handleEndCommand()
{
    // Handle END command
    Log.noticeln(F("react_engine: Command END"));
    pc = 0;
}

/**
 * @brief Handle the WAIT_EVENT command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleWaitEventCommand(uint8_t *bytecode)
{
    re_state = RE_HOLD_WAIT_APP_EVENT;
    waited_app_event = bytecode[pc];
    waited_app_event_time = millis();
    pc = pc + 1;
    Log.noticeln(F("react_engine: Command WAIT_EVENT, event: %i"), waited_app_event);
    event_registry_enable_app_event();
}

/**
 * @brief Handle the LED_COLOR command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleLedColorCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command LED_COLOR"));
    uint32_t color = ((uint32_t)bytecode[pc] << 16);
    color |= ((uint32_t)bytecode[pc + 1] << 8);
    color |= bytecode[pc + 2];
    pc = pc + 3;
    led_set_color(color);
}

/**
 * @brief Handle asynchronous commands with parameters.
 *
 * @param commandCode The code of the command.
 * @param eventType The type of event associated with the command.
 * @param handlerFunction The function to handle the command.
 * @param params An array of two parameters for the handler function.
 */
void handleAsyncCommand(uint8_t commandCode, EVENT_TYPE eventType, void (*handlerFunction)(uint32_t *params), uint32_t params[2])
{
    ASYNC_COMMANDS *ac = asyncCommandsList.getItem(commandCode, eventType);

    if (ac == nullptr)
    {
        // Create a new ASYNC_COMMANDS object and add it to the list
        ac = new ASYNC_COMMANDS;
        ac->bytecode = commandCode;
        ac->active = true;
        ac->event_type = eventType;
        ac->handler_function = handlerFunction;
        // Copy the values from params to ac->params
        for (int i = 0; i < 2; i++)
        {
            ac->params[i] = params[i];
        }
        asyncCommandsList.add(*ac); // Add the object, not the pointer
        Log.noticeln(F("react_engine: adding handler for event %i"), ac->event_type);
    }
    else
    {
        // Update the existing ASYNC_COMMANDS object
        ac->active = true;
        ac->event_type = eventType;
        ac->handler_function = handlerFunction;
        // Copy the values from params to ac->params
        for (int i = 0; i < 2; i++)
        {
            ac->params[i] = params[i];
        }
        Log.noticeln(F("react_engine: updating handler for event %i"), ac->event_type);
    }
}

/**
 * @brief Handle the FOOT_PRESS_COLOR commands.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 * @param eventType The event type to use for ASYNC_COMMANDS.
 */
void handleFootPressColorCommand(uint8_t *bytecode, EVENT_TYPE eventPress, EVENT_TYPE eventRelease)
{
    uint8_t command = bytecode[pc - 1];
    uint32_t press_color = ((uint32_t)bytecode[pc] << 16);
    press_color |= ((uint32_t)bytecode[pc + 1] << 8);
    press_color |= bytecode[pc + 2];
    pc = pc + 3;
    uint32_t release_color = ((uint32_t)bytecode[pc] << 16);
    release_color |= ((uint32_t)bytecode[pc + 1] << 8);
    release_color |= bytecode[pc + 2];
    pc = pc + 3;
    // Define parameters for the handler function
    uint32_t params[2] = {press_color, 0};
    // Handle ASYNC_COMMANDS separately with parameters
    handleAsyncCommand(command, eventPress, &change_react_device_color, params);
    params[0] = release_color;
    handleAsyncCommand(command, eventRelease, &change_react_device_color, params);
}

/**
 * @brief Handle the LED_TRAFFIC_LIGHT command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleLedTrafficLightCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command LED_TRAFFIC_LIGHT"));
    // Implement the functionality for LED_TRAFFIC_LIGHT command here
}

/**
 * @brief Handle the TIMER command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleTimerCommand(uint8_t *bytecode)
{
    // Handle TIMER command with args
    Log.noticeln(F("react_engine: Command TIMER %l"), millis());
    timer = (uint16_t)(bytecode[pc] << 8);
    timer |= bytecode[pc + 1]; 
    timer_display = (bytecode[pc + 2] == ARGS::TRUE) ? true : false;
    pc = pc + 3;
    re_state = RE_HOLD_TIMER_ENABLE;
    if (timer_display)
    {
        display_number(timer);
        // removing all other display 
        ASYNC_COMMANDS *ac = asyncCommandsList.getItem(EVENT_APP_TYPE_FOOT_PRESS_LEFT);
        if (ac != nullptr)
        {
            ac->active = false;
            display_clear();
        }
    }
}

/**
 * @brief Handle the FOOT_PRESS_COUNTER command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleFootPressCounterCommand(uint8_t *bytecode)
{
    uint8_t command = bytecode[pc - 1];
    Log.noticeln(F("react_engine: Command FOOT_PRESS_COUNTER"));
    bool enable = (bytecode[pc] == ARGS::TRUE) ? true : false;
    pc = pc +1;
    uint32_t params[2] = {enable, 0};
    handleAsyncCommand(command, EVENT_APP_TYPE_FOOT_PRESS_LEFT, &update_react_device_foot_press_counter, params);
    handleAsyncCommand(command, EVENT_APP_TYPE_FOOT_PRESS_RIGHT, &update_react_device_foot_press_counter, params);
    timer_display =false;
    display_clear();
    if (enable == true)
    {
        display_number(foot_press_counter);
    }
    // Implement the functionality for FOOT_PRESS_COUNTER command here
}

/**
 * @brief Handle the FOOT_PRESS_COUNTER_RESET command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleFootPressCounterResetCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command FOOT_PRESS_COUNTER_RESET"));
    foot_press_counter = 0;
    display_number(0);
}

/**
 * @brief Interpret and execute commands with arguments.
 *
 * @param bytecode The bytecode containing the command to interpret and execute.
 */
void interpret_command(uint8_t *bytecode)
{
    uint8_t command = bytecode[pc];
    last_pc = pc;
    pc = pc + 1;
    switch (command)
    {
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
        Log.noticeln(F("react_engine: Command FOOT_PRESS_LEFT_COLOR"));
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_LEFT, EVENT_APP_TYPE_FOOT_RELEASE_LEFT);
        break;
    case FOOT_PRESS_RIGHT_COLOR:
        Log.noticeln(F("react_engine: Command FOOT_PRESS_RIGHT_COLOR"));
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_RIGHT, EVENT_APP_TYPE_FOOT_RELEASE_RIGHT);
        break;
    case FOOT_PRESS_COLOR:
        Log.noticeln(F("react_engine: Command FOOT_PRESS_LEFT_COLOR"));
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_LEFT, EVENT_APP_TYPE_FOOT_RELEASE_LEFT);
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_RIGHT, EVENT_APP_TYPE_FOOT_RELEASE_RIGHT);
        break;
    case FOOT_PRESS_COUNTER:
        handleFootPressCounterCommand(bytecode);
        break;
    case FOOT_PRESS_COUNTER_RESET:
        handleFootPressCounterResetCommand(bytecode);
        break;

    default:
        Log.noticeln(F("react_engine: Command UNKNOWN COMMAND: %i"), command);
        // Unknown command
        break;
    }
}

/**
 * @brief Initialize the react engine.
 */
void react_engine_setup()
{
    Log.notice(F("react_engine: setup" CR));
    re_state = RE_READY;
}

/**
 * @brief Stop the react engine and reset its state.
 */
void react_engine_stop()
{
    re_state = RE_READY;
    pc = 0;
    waited_app_event = 0;
    event_registry_disable_app_event();
    led_set_color(COLOR_BLACK);
}

/**
 * @brief Pause the react engine, disabling app events.
 */
void react_engine_pause()
{
    re_state = RE_PAUSE;
    event_registry_disable_app_event();
}

/**
 * @brief Main task of the react engine, handles the execution of commands and timers.
 */
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
            if (timer_display)
            {
                display_number(timer);
            }
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
