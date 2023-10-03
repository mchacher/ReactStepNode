#include <Arduino.h>
#include <ArduinoLog.h>
#include "react_engine.h"
#include "led.h"
#include "event_type.h"
#include "event_registry.h"
#include "react_code.h"
#include "async_commands_list.h"
#include "display.h"

// uint8_t react_code[] =
//     {
//         START,
//         LED_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF,
//         TIMER_HOLD, 0, 3, ARGS::TRUE,
//         LED_EFFECT, LED_EFFECTS::EFFECT_MUSIC,
//         TIMER_HOLD, 0, 5, ARGS::TRUE,
//         LED_COLOR, (COLOR_RED >> 16) & 0xFF, (COLOR_RED >> 8) & 0xFF, COLOR_RED & 0xFF,
//         WAIT_EVENT, EVENT_APP_TYPE_FOOT_PRESS_RIGHT,
//         FOOT_PRESS_LEFT_COLOR, (COLOR_PINK >> 16) & 0xFF, (COLOR_PINK >> 8) & 0xFF, COLOR_PINK & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
//         FOOT_PRESS_RIGHT_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
//         FOOT_PRESS_COUNTER_RESET,
//         FOOT_PRESS_COUNTER, ARGS::TRUE,
//         TIMER_HOLD, 0, 5, ARGS::FALSE,
//         FOOT_PRESS_LEFT_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
//         FOOT_PRESS_RIGHT_COLOR, (COLOR_PINK >> 16) & 0xFF, (COLOR_PINK >> 8) & 0xFF, COLOR_PINK & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
//         FOOT_PRESS_COUNTER, ARGS::TRUE,
//         TIMER_HOLD, 0, 5, ARGS::TRUE,
//         LED_COLOR, (COLOR_BLUE >> 16) & 0xFF, (COLOR_BLUE >> 8) & 0xFF, COLOR_BLUE & 0xFF,
//         WAIT_EVENT, EVENT_APP_TYPE_FOOT_PRESS_LEFT,
//         END};

// uint8_t react_code[] =
//     {
//         START,
//         LED_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF,
//         TIMER_HOLD, 0, 3, ARGS::TRUE,
//         LED_EFFECT, LED_EFFECTS::EFFECT_MUSIC,
//         TIMER_HOLD, 0, 5, ARGS::TRUE,
//         END,
//     };

// const uint8_t react_code[] = {
//     0x80, 0xA0, 0x00, 0x80, 0x00, 0x83, 0x00, 0x03, 0x20, 0xA2, 0x23, 0x83, 0x00, 0x05,
//     0x20, 0xA0, 0xFF, 0x00, 0x00, 0x82, 0x42, 0xB1, 0xFF, 0x20, 0xCC, 0x00, 0x00, 0x00,
//     0xB2, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0xB6, 0xB3, 0x20, 0x83, 0x00, 0x05, 0x20,
//     0xB1, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xFF, 0x20, 0xCC, 0x00, 0x00, 0x00,
//     0xB3, 0x20, 0x83, 0x00, 0x05, 0x20, 0xA0, 0x00, 0x00, 0xFF, 0x82, 0x41, 0x81
// };

// below is a kind of TRAFFIC LIGHT code to illustrate TIMER non blocking command
const uint8_t react_code[256] =
    {
        START,
        LED_COLOR, (COLOR_RED >> 16) & 0xFF, (COLOR_RED >> 8) & 0xFF, COLOR_RED & 0xFF,
        TIMER, 0, 3, ARGS::TRUE,
        WAIT_EVENT, EVENT_APP_TIMER, 0, 2,
        LED_COLOR, (COLOR_ORANGE >> 16) & 0xFF, (COLOR_ORANGE >> 8) & 0xFF, COLOR_ORANGE & 0xFF,
        WAIT_EVENT, EVENT_APP_TIMER, 0, 1,
        LED_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF,
        WAIT_EVENT, EVENT_APP_TIMER, 0, 0,
        LED_COLOR, (COLOR_WHITE >> 16) & 0xFF, (COLOR_WHITE >> 8) & 0xFF, COLOR_WHITE & 0xFF,
        WAIT_EVENT, EVENT_APP_TYPE_FOOT_PRESS_LEFT,
        END};

static int pc = 0;
static int last_pc = 0;
static unsigned long last_command_time = 0;
static uint16_t timer = 0;
static EVENT waited_app_event;
static unsigned long waited_app_event_time;
static RE_STATE re_state = RE_IDLE;
static uint16_t tick = 0;
static uint16_t foot_press_counter = 0;
static bool timer_display = false;

#define MAX_ASYNC_COMMANDS 10
AsyncCommandsList<MAX_ASYNC_COMMANDS> asyncCommandsList;

/**
 * @brief Change the color of the LED based on parameters. This is an Async React Command.
 *
 * @param params param 0 - the color to apply
 */
void change_react_device_color(uint32_t *params)
{
    Log.noticeln(F("react_engine: change_react_device_color"));
    led_set_color(params[0]);
}

/**
 * @brief Update foot press counter. This is an Async React Command.
 *
 * @param params no paramater
 */
void update_react_device_foot_press_counter(uint32_t *params)
{
    Log.verboseln(F("react_engine: update_foot_press_counter"));
    foot_press_counter++;
    if (params[0])
    {
        display_number(foot_press_counter);
    }
}

/**
 * @brief Handle application events by checking the event registry and executing corresponding async commands.
 */
void handle_application_events()
{
    EVENT app_event;
    if (event_registry_pop_app_event(app_event))
    {
        Log.verboseln(F("react_engine: handle_application_events for event %i"), app_event.type);
        const size_t MAX_RESULTS = 10;
        ASYNC_COMMANDS *resultItems[MAX_RESULTS];
        size_t itemCount = asyncCommandsList.getItemsByEventType((EVENT_TYPE)app_event.type, resultItems, MAX_RESULTS);
        Log.verboseln(F("react_engine: ---- itemCount: %i"), itemCount);
        for (size_t i = 0; i < itemCount; i++)
        {

            if (resultItems[i]->active == true)
            {
                resultItems[i]->handler_function(resultItems[i]->params);
            }
        }
    }
}

/**
 * @brief Handle asynchronous commands with parameters. Add or update async commands to the list.
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
 * @brief Handle the START command.
 */
void handleStartCommand()
{
    // Handle START command
    Log.noticeln(F("---------------------------"));
    Log.noticeln(F("react_engine: Command START - %l ms"), millis() - last_command_time);
    last_command_time = millis();
}

/**
 * @brief Handle the END command.
 */
void handleEndCommand()
{
    // Handle END command
    Log.noticeln(F("react_engine: Command END - %l ms"), millis() - last_command_time);
    last_command_time = millis();
    pc = 0;
}

/**
 * @brief Handle the WAIT_EVENT command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleWaitEventCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command WAIT_EVENT, event = %i - %l ms"), waited_app_event, millis() - last_command_time);
    last_command_time = millis();
    re_state = RE_HOLD_WAIT_APP_EVENT;
    waited_app_event.type = (EVENT_TYPE)bytecode[pc];
    waited_app_event.timestamp = millis();
    pc = pc + 1;
    // if EVENT_APP_TIMER, last argument is the timer value in s (16 bits)
    if (waited_app_event.type == EVENT_TYPE::EVENT_APP_TIMER)
    {
        waited_app_event.value = ((uint32_t)bytecode[pc] << 8);
        waited_app_event.value |= bytecode[pc + 1];
        Log.noticeln(F("react_engine: Command WAIT_EVENT - EVENT_APP_TIMER with timer value = %is "), waited_app_event.value);
        pc = pc + 2;
    }
    event_registry_enable_app_event();
}

/**
 * @brief Handle the LED_COLOR command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleLedColorCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command LED_COLOR - %l ms"), millis() - last_command_time);
    last_command_time = millis();
    uint32_t color = ((uint32_t)bytecode[pc] << 16);
    color |= ((uint32_t)bytecode[pc + 1] << 8);
    color |= bytecode[pc + 2];
    pc = pc + 3;
    led_set_color(color);
}

/**
 * @brief Handle the FOOT_PRESS_COLOR commands.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 * @param eventType The event type to use for ASYNC_COMMANDS.
 */
void handleFootPressColorCommand(uint8_t *bytecode, EVENT_TYPE eventPress, EVENT_TYPE eventRelease)
{
    Log.noticeln(F("react_engine: Command FOOT_PRESS_LEFT_COLOR - %l ms"), millis() - last_command_time);
    last_command_time = millis();
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
    Log.noticeln(F("react_engine: Command LED_TRAFFIC_LIGHT - %l ms"), millis() - last_command_time);
    last_command_time = millis();
    // Implement the functionality for LED_TRAFFIC_LIGHT command here
}

/**
 * @brief Handle the TIMER_HOLD command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleTimerHoldCommand(uint8_t *bytecode)
{
    // Handle TIMER_HOLD command with args
    Log.noticeln(F("react_engine: Command TIMER_HOLD %l ms"), millis() - last_command_time);
    last_command_time = millis();
    timer = (uint16_t)(bytecode[pc] << 8);
    timer |= bytecode[pc + 1];
    timer_display = (bytecode[pc + 2] == ARGS::TRUE) ? true : false;
    pc = pc + 3;
    re_state = RE_HOLD_TIMER_ENABLE;
    if (timer_display)
    {
        // removing all other display
        const size_t MAX_RESULTS = 10;
        ASYNC_COMMANDS *resultItems[MAX_RESULTS];
        size_t itemCount = asyncCommandsList.getItemsByCommandCode(COMMANDS::FOOT_PRESS_COUNTER, resultItems, MAX_RESULTS);
        for (size_t i = 0; i < itemCount; i++)
        {
            resultItems[i]->active = false;
        }
        display_number(timer);
    }
}

/**
 * @brief Handle the TIMER command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleTimerCommand(uint8_t *bytecode)
{
    // Handle TIMER command with args
    Log.noticeln(F("react_engine: Command TIMER %l ms"), millis() - last_command_time);
    last_command_time = millis();
    timer = (uint16_t)(bytecode[pc] << 8);
    timer |= bytecode[pc + 1];
    timer_display = (bytecode[pc + 2] == ARGS::TRUE) ? true : false;
    pc = pc + 3;
    if (timer_display)
    {
        // removing all other display
        const size_t MAX_RESULTS = 10;
        ASYNC_COMMANDS *resultItems[MAX_RESULTS];
        size_t itemCount = asyncCommandsList.getItemsByCommandCode(COMMANDS::FOOT_PRESS_COUNTER, resultItems, MAX_RESULTS);
        for (size_t i = 0; i < itemCount; i++)
        {
            resultItems[i]->active = false;
        }
        display_number(timer);
    }
}

/**
 * @brief Handle the FOOT_PRESS_COUNTER command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleFootPressCounterCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command FOOT_PRESS_COUNTER - %l ms"), millis() - last_command_time);
    last_command_time = millis();
    bool enable = (bytecode[pc] == ARGS::TRUE) ? true : false;
    pc = pc + 1;
    uint32_t params[2] = {enable, 0};
    handleAsyncCommand(COMMANDS::FOOT_PRESS_COUNTER, EVENT_APP_TYPE_FOOT_PRESS_LEFT, &update_react_device_foot_press_counter, params);
    handleAsyncCommand(COMMANDS::FOOT_PRESS_COUNTER, EVENT_APP_TYPE_FOOT_PRESS_RIGHT, &update_react_device_foot_press_counter, params);
    timer_display = false;
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
    Log.noticeln(F("react_engine: Command FOOT_PRESS_COUNTER_RESET - %l ms"), millis() - last_command_time);
    last_command_time = millis();
    foot_press_counter = 0;
    display_number(0);
}

/**
 * @brief Handle the LED_EFFECT command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleLedEffectCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command LED_EFFECT - %l ms"), millis() - last_command_time);
    last_command_time = millis();
    uint8_t effect = bytecode[pc];
    pc = pc + 1;
    led_set_effect((LED_EFFECTS)effect);
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
    case TIMER_HOLD:
        handleTimerHoldCommand(bytecode);
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
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_LEFT, EVENT_APP_TYPE_FOOT_RELEASE_LEFT);
        break;
    case FOOT_PRESS_RIGHT_COLOR:
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_RIGHT, EVENT_APP_TYPE_FOOT_RELEASE_RIGHT);
        break;
    case FOOT_PRESS_COLOR:
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_LEFT, EVENT_APP_TYPE_FOOT_RELEASE_LEFT);
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_RIGHT, EVENT_APP_TYPE_FOOT_RELEASE_RIGHT);
        break;
    case FOOT_PRESS_COUNTER:
        handleFootPressCounterCommand(bytecode);
        break;
    case FOOT_PRESS_COUNTER_RESET:
        handleFootPressCounterResetCommand(bytecode);
        break;
    case LED_EFFECT:
        handleLedEffectCommand(bytecode);
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
    waited_app_event.type = EVENT_TYPE_NONE;
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

void task_timer()
{
    static EVENT e;
    if (timer > 0)
    {
        tick++;
        if ((tick * REACT_ENGINE_CYCLE_TIME) == 1000) // 1s
        {
            timer = timer - 1;
            // raise event every s when timer is updated
            e.timestamp = millis();
            e.type = EVENT_APP_TIMER;
            e.value = timer;
            event_registry_push_app_event(e);
            if (timer_display)
            {
                display_number(timer);
            }
            tick = 0;
        }
    }
}

/**
 * @brief Main task of the react engine, handles the execution of commands and timers.
 */
void react_engine_task()
{
    task_timer();
    switch (re_state)
    {
    case RE_READY:
        re_state = RE_RUN;
        event_registry_enable_app_event();
    case RE_RUN:
        interpret_command(react_code);
        handle_application_events();
        break;
    case RE_HOLD_TIMER_ENABLE:
        handle_application_events();
        if (timer == 0)
        {
            re_state = RE_RUN;
        }
        // tick++;
        // if ((tick * REACT_ENGINE_CYCLE_TIME) == 1000) // 1s
        // {
        //     timer = timer - 1;
        //     if (timer_display)
        //     {
        //         display_number(timer);
        //     }
        //     if (timer == 0)
        //     {
        //         re_state = RE_RUN;
        //     }
        //     tick = 0;
        // }
        break;
    case RE_HOLD_WAIT_APP_EVENT:
        EVENT app_event;
        if (waited_app_event.type == EVENT_APP_TYPE_FOOT_PRESS)
        {
            if (event_registry_get_app_event(EVENT_APP_TYPE_FOOT_PRESS_LEFT, waited_app_event.timestamp, app_event) || event_registry_get_app_event(EVENT_APP_TYPE_FOOT_PRESS_RIGHT, waited_app_event.timestamp, app_event))
            {
                re_state = RE_RUN;
            }
        }
        else if (event_registry_get_app_event(waited_app_event.type, waited_app_event_time, app_event))
        {
            if ((waited_app_event.type != EVENT_TYPE::EVENT_APP_TIMER) || ((waited_app_event.type == EVENT_TYPE::EVENT_APP_TIMER) && (waited_app_event.value == app_event.value)))
            {
                Log.noticeln(F("react_engine_task: switching to RUN after event: %i"), waited_app_event.type);
                re_state = RE_RUN;
            }
        }
        break;
    case RE_PAUSE:
        pc = last_pc;
        re_state = RE_READY;
    }
}
