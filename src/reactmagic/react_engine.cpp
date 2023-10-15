#include <Arduino.h>
#include <ArduinoLog.h>
#include "react_engine.h"
#include "../hmi/led.h"
#include "event_type.h"
#include "event_registry.h"
#include "react_code.h"
#include "async_commands_list.h"
#include "../hmi/display.h"

// uint8_t react_code[] =
//     {
//         CMD_START,
//         CMD_LED_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF,
//         CMD_TIMER_HOLD, 0, 3, ARGS::TRUE,
//         CMD_LED_EFFECT, LED_EFFECTS::EFFECT_MUSIC,
//         CMD_TIMER_HOLD, 0, 5, ARGS::TRUE,
//         CMD_LED_COLOR, (COLOR_RED >> 16) & 0xFF, (COLOR_RED >> 8) & 0xFF, COLOR_RED & 0xFF,
//         CMD_WAIT_EVENT, EVENT_APP_TYPE_FOOT_PRESS_RIGHT,
//         CMD_FOOT_PRESS_LEFT_COLOR, (COLOR_PINK >> 16) & 0xFF, (COLOR_PINK >> 8) & 0xFF, COLOR_PINK & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
//         CMD_FOOT_PRESS_RIGHT_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
//         CMD_FOOT_PRESS_COUNTER_RESET,
//         CMD_FOOT_PRESS_COUNTER, ARGS::TRUE,
//         CMD_TIMER_HOLD, 0, 5, ARGS::FALSE,
//         CMD_FOOT_PRESS_LEFT_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
//         CMD_FOOT_PRESS_RIGHT_COLOR, (COLOR_PINK >> 16) & 0xFF, (COLOR_PINK >> 8) & 0xFF, COLOR_PINK & 0xFF, (COLOR_BLACK >> 16) & 0xFF, (COLOR_BLACK >> 8) & 0xFF, COLOR_BLACK & 0xFF,
//         CMD_FOOT_PRESS_COUNTER, ARGS::TRUE,
//         CMD_TIMER_HOLD, 0, 5, ARGS::TRUE,
//         CMD_LED_COLOR, (COLOR_BLUE >> 16) & 0xFF, (COLOR_BLUE >> 8) & 0xFF, COLOR_BLUE & 0xFF,
//         CMD_WAIT_EVENT, EVENT_APP_TYPE_FOOT_PRESS_LEFT,
//         CMD_END};

// uint8_t react_code[] =
//     {
//         CMD_START,
//         CMD_LED_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF,
//         CMD_TIMER_HOLD, 0, 3, ARGS::TRUE,
//         CMD_LED_EFFECT, LED_EFFECTS::EFFECT_MUSIC,
//         CMD_TIMER_HOLD, 0, 5, ARGS::TRUE,
//         CMD_END,
//     };

// uint8_t react_code[] =
//     {
//         CMD_START,
//         CMD_REPEAT_COUNT, 0, 3, ARGS::TRUE,
//         CMD_LED_COLOR, (COLOR_GREEN >> 16) & 0xFF,(COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF,
//         CMD_TIMER_HOLD, 0, 1, ARGS::FALSE,
//         CMD_LED_COLOR, (COLOR_RED >> 16) & 0xFF, (COLOR_RED >> 8) & 0xFF, COLOR_RED & 0xFF,
//         CMD_TIMER_HOLD, 0, 1, ARGS::FALSE,
//         CMD_REPEAT_END,
//         CMD_LED_COLOR, (COLOR_BLUE >> 16) & 0xFF, (COLOR_BLUE >> 8) & 0xFF, COLOR_BLUE & 0xFF,
//         CMD_WAIT_EVENT, EVENT_APP_TYPE_FOOT_PRESS_LEFT,
//         CMD_END,
// };

// const uint8_t react_code[] = {
// 0x80, 0x85, 0x00, 0x03, 0x20, 0xA0, 0xFF, 0xFF, 
// 0x00, 0x83, 0x00, 0x02, 0x21, 0x85, 0x00, 0x02, 
// 0x21, 0xA0, 0x00, 0x80, 0x00, 0x83, 0x00, 0x02, 
// 0x21, 0xA0, 0xFF, 0x00, 0x00, 0x83, 0x00, 0x02, 
// 0x21, 0x86, 0x86, 0xA0, 0x00, 0x00, 0xFF, 0x82, 
// 0x41, 0x81
// };

// const uint8_t react_code[] = {
//     0x80, 0xA0, 0x00, 0x80, 0x00, 0x83, 0x00, 0x03, 0x20, 0xA2, 0x23, 0x83, 0x00, 0x05,
//     0x20, 0xA0, 0xFF, 0x00, 0x00, 0x82, 0x42, 0xB1, 0xFF, 0x20, 0xCC, 0x00, 0x00, 0x00,
//     0xB2, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0xB6, 0xB3, 0x20, 0x83, 0x00, 0x05, 0x20,
//     0xB1, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xFF, 0x20, 0xCC, 0x00, 0x00, 0x00,
//     0xB3, 0x20, 0x83, 0x00, 0x05, 0x20, 0xA0, 0x00, 0x00, 0xFF, 0x82, 0x41, 0x81};

// below is a kind of TRAFFIC LIGHT code to illustrate TIMER non blocking command
const uint8_t react_code[256] =
    {
        CMD_START,
        CMD_TIMER, 0, 3, ARGS::TRUE,
        CMD_LED_COLOR, (COLOR_RED >> 16) & 0xFF, (COLOR_RED >> 8) & 0xFF, COLOR_RED & 0xFF,
        CMD_WAIT_EVENT, EVENT_APP_TIMER, 0, 2,
        CMD_LED_COLOR, (COLOR_ORANGE >> 16) & 0xFF, (COLOR_ORANGE >> 8) & 0xFF, COLOR_ORANGE & 0xFF,
        CMD_WAIT_EVENT, EVENT_APP_TIMER, 0, 1,
        CMD_LED_COLOR, (COLOR_GREEN >> 16) & 0xFF, (COLOR_GREEN >> 8) & 0xFF, COLOR_GREEN & 0xFF,
        CMD_WAIT_EVENT, EVENT_APP_TIMER, 0, 0,
        CMD_LED_COLOR, (COLOR_WHITE >> 16) & 0xFF, (COLOR_WHITE >> 8) & 0xFF, COLOR_WHITE & 0xFF,
        CMD_WAIT_EVENT, EVENT_APP_TYPE_FOOT_PRESS_LEFT,
        CMD_END};



static const int MAX_REPEAT_DEPTH = 5;
static RepeatContext repeat_context[MAX_REPEAT_DEPTH];
static uint8_t repeat_index = 0;
static Context context;
static Context saved_context;

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
    context.foot_press_counter++;
    if (params[0])
    {
        display_number(context.foot_press_counter);
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
    Log.noticeln(F("react_engine: Command START"));
}

/**
 * @brief Handle the END command.
 */
void handleEndCommand()
{
    // Handle END command
    Log.noticeln(F("react_engine: Command END"));
    event_registry_push(EVENT_SYS_TYPE_DONE);
    context.state = RE_DONE;
}

/**
 * @brief Handle the WAIT_EVENT command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleWaitEventCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command WAIT_EVENT, event = %i"), context.waited_app_event);
    context.state = RE_HOLD_WAIT_APP_EVENT;
    context.waited_app_event.type = (EVENT_TYPE)bytecode[context.pc];
    context.waited_app_event.timestamp = millis();
    context.pc = context.pc + 1;
    // if EVENT_APP_TIMER, last argument is the timer value in s (16 bits)
    if (context.waited_app_event.type == EVENT_TYPE::EVENT_APP_TIMER)
    {
        context.waited_app_event.value = ((uint32_t)bytecode[context.pc] << 8);
        context.waited_app_event.value |= bytecode[context.pc + 1];
        Log.noticeln(F("react_engine: Command WAIT_EVENT - EVENT_APP_TIMER with timer value = %is "), context.waited_app_event.value);
        context.pc = context.pc + 2;
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
    Log.noticeln(F("react_engine: Command LED_COLOR"));
    uint32_t color = ((uint32_t)bytecode[context.pc] << 16);
    color |= ((uint32_t)bytecode[context.pc + 1] << 8);
    color |= bytecode[context.pc + 2];
    context.pc = context.pc + 3;
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
    Log.noticeln(F("react_engine: Command FOOT_PRESS_LEFT_COLOR"));
    uint8_t command = bytecode[context.pc - 1];
    uint32_t press_color = ((uint32_t)bytecode[context.pc] << 16);
    press_color |= ((uint32_t)bytecode[context.pc + 1] << 8);
    press_color |= bytecode[context.pc + 2];
    context.pc = context.pc + 3;
    uint32_t release_color = ((uint32_t)bytecode[context.pc] << 16);
    release_color |= ((uint32_t)bytecode[context.pc + 1] << 8);
    release_color |= bytecode[context.pc + 2];
    context.pc = context.pc + 3;
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
    Log.noticeln(F("react_engine: Command TIMER"));
    context.timer = (uint16_t)(bytecode[context.pc] << 8);
    context.timer |= bytecode[context.pc + 1];
    context.timer_display = (bytecode[context.pc + 2] == ARGS::TRUE) ? true : false;
    context.pc = context.pc + 3;
    if (context.timer_display)
    {
        // removing all other display
        const size_t MAX_RESULTS = 10;
        ASYNC_COMMANDS *resultItems[MAX_RESULTS];
        size_t itemCount = asyncCommandsList.getItemsByCommandCode(CMD_FOOT_PRESS_COUNTER, resultItems, MAX_RESULTS);
        for (size_t i = 0; i < itemCount; i++)
        {
            resultItems[i]->active = false;
        }
        display_number(context.timer);
    }
}

/**
 * @brief Handle the TIMER_HOLD command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleTimerHoldCommand(uint8_t *bytecode)
{
    // Handle TIMER_HOLD command with args
    Log.noticeln(F("react_engine: Command TIMER_HOLD (calling TIMER command and switching to HOLD state"));
    handleTimerCommand(bytecode);
    context.state = RE_HOLD_TIMER_ENABLE;
}

/**
 * @brief Handle the FOOT_PRESS_COUNTER command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleFootPressCounterCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command FOOT_PRESS_COUNTER"));
    bool enable = (bytecode[context.pc] == ARGS::TRUE) ? true : false;
    context.pc = context.pc + 1;
    uint32_t params[2] = {enable, 0};
    handleAsyncCommand(CMD_FOOT_PRESS_COUNTER, EVENT_APP_TYPE_FOOT_PRESS_LEFT, &update_react_device_foot_press_counter, params);
    handleAsyncCommand(CMD_FOOT_PRESS_COUNTER, EVENT_APP_TYPE_FOOT_PRESS_RIGHT, &update_react_device_foot_press_counter, params);
    context.timer_display = false;
    display_clear();
    if (enable == true)
    {
        display_number(context.foot_press_counter);
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
    context.foot_press_counter = 0;
    display_number(0);
}

/**
 * @brief Handle the LED_EFFECT command.
 *
 * @param bytecode The bytecode containing the command and its arguments.
 */
void handleLedEffectCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command LED_EFFECT"));
    uint8_t effect = bytecode[context.pc];
    context.pc = context.pc + 1;
    led_set_effect((LED_EFFECTS)effect);
}

void handleRepeatCountCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command REPEAT_COUNT"));
    repeat_context[repeat_index].count = (uint16_t)(bytecode[context.pc] << 8);
    repeat_context[repeat_index].count |= bytecode[context.pc + 1];
    repeat_context[repeat_index].display = (bytecode[context.pc + 2] == ARGS::TRUE) ? true : false;
    // TODO manage centrally all types of display
    if (repeat_context[repeat_index].display == true)
    {
        display_number(repeat_context[repeat_index].count);
    }
    context.pc = context.pc + 3;
    repeat_context[repeat_index].pc = context.pc;
    repeat_index++;
}

void handleRepeatEndCommand(uint8_t *bytecode)
{
    Log.noticeln(F("react_engine: Command REPEAT_END"));
    repeat_context[repeat_index-1].count--;
    if (repeat_context[repeat_index-1].count != 0)
    {
        context.pc = repeat_context[repeat_index-1].pc;
    }
    else if (repeat_index > 1)
    {
        repeat_index--;
    }
    if (repeat_context[repeat_index-1].display == true)
    {
        display_number(repeat_context[repeat_index-1].count);
    }
}

/**
 * @brief Interpret and execute commands with arguments.
 *
 * @param bytecode The bytecode containing the command to interpret and execute.
 */
void interpret_command(uint8_t *bytecode)
{
    uint8_t command = bytecode[context.pc];
    context.pc = context.pc + 1;
    switch (command)
    {
    case CMD_START:
        handleStartCommand();
        break;
    case CMD_END:
        handleEndCommand();
        break;
    case CMD_WAIT_EVENT:
        handleWaitEventCommand(bytecode);
        break;
    case CMD_TIMER_HOLD:
        handleTimerHoldCommand(bytecode);
        break;
    case CMD_TIMER:
        handleTimerCommand(bytecode);
        break;
    case CMD_REPEAT_COUNT:
        handleRepeatCountCommand(bytecode);
        break;
    case CMD_REPEAT_END:
        handleRepeatEndCommand(bytecode);
        break;
    case CMD_SEND_EVENT:
        Log.noticeln(F("react_engine: Command SEND_EVENT NOT IMPLEMENTED"));
        // Handle SEND command
        break;
    case CMD_LED_COLOR:
        handleLedColorCommand(bytecode);
        break;
    case CMD_FOOT_PRESS_LEFT_COLOR:
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_LEFT, EVENT_APP_TYPE_FOOT_RELEASE_LEFT);
        break;
    case CMD_FOOT_PRESS_RIGHT_COLOR:
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_RIGHT, EVENT_APP_TYPE_FOOT_RELEASE_RIGHT);
        break;
    case CMD_FOOT_PRESS_COLOR:
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_LEFT, EVENT_APP_TYPE_FOOT_RELEASE_LEFT);
        handleFootPressColorCommand(bytecode, EVENT_APP_TYPE_FOOT_PRESS_RIGHT, EVENT_APP_TYPE_FOOT_RELEASE_RIGHT);
        break;
    case CMD_FOOT_PRESS_COUNTER:
        handleFootPressCounterCommand(bytecode);
        break;
    case CMD_FOOT_PRESS_COUNTER_RESET:
        handleFootPressCounterResetCommand(bytecode);
        break;
    case CMD_LED_EFFECT:
        handleLedEffectCommand(bytecode);
        break;
    default:
        Log.noticeln(F("react_engine: Command UNKNOWN COMMAND: %i"), command);
        // Unknown command
        break;
    }
}


void react_engine_clear_context()
{
    context.pc = 0;
    context.state = RE_IDLE;
    context.foot_press_counter = 0;
    context.timer = 0;
    context.timer_display = false;
    context.waited_app_event_time = 0;
    context.waited_app_event.type = EVENT_TYPE_NONE;
    asyncCommandsList.clear();
}
/**
 * @brief Initialize the react engine.
 */
void react_engine_setup()
{
    Log.noticeln(F("react_engine: setup"));
    react_engine_clear_context();
    context.state = RE_READY;
}

/**
 * @brief Stop the react engine and reset its state.
 */
void react_engine_stop()
{
    react_engine_clear_context();
    context.state = RE_READY;
    event_registry_disable_app_event();
    led_set_color(COLOR_BLACK);
}

/**
 * @brief Pause the react engine, disabling app events.
 */
void react_engine_pause()
{
    saved_context = context;
    event_registry_disable_app_event();
    context.state = RE_PAUSE;
}

void react_engine_process_timer()
{
    static EVENT e;
    static uint16_t tick = 0;
    if (context.timer > 0)
    {
        if ((tick * REACT_ENGINE_CYCLE_TIME) == 1000) // 1s
        {
            context.timer = context.timer - 1;
            // raise event every s when timer is updated
            e.timestamp = millis();
            e.type = EVENT_APP_TIMER;
            e.value = context.timer;
            event_registry_push_app_event(e);
            tick = 0;
            if (context.timer_display)
            {
                display_number(context.timer);
            }
        }
        tick++;
    }
}

/**
 * @brief Main task of the react engine, handles the execution of commands and timers.
 */
void react_engine_task()
{
    react_engine_process_timer();
    switch (context.state)
    {
    case RE_READY:
        context.state = RE_RUN;
        event_registry_enable_app_event();
        break;
    case RE_RUN:
        interpret_command((uint8_t *)react_code);
        handle_application_events();
        break;
    case RE_HOLD_TIMER_ENABLE:
        handle_application_events();
        if (context.timer == 0)
        {
            context.state = RE_RUN;
        }
        break;
    case RE_HOLD_WAIT_APP_EVENT:
        EVENT app_event;
        if (context.waited_app_event.type == EVENT_APP_TYPE_FOOT_PRESS)
        {
            if (event_registry_get_app_event(EVENT_APP_TYPE_FOOT_PRESS_LEFT, context.waited_app_event.timestamp, app_event) || event_registry_get_app_event(EVENT_APP_TYPE_FOOT_PRESS_RIGHT, context.waited_app_event.timestamp, app_event))
            {
                context.state = RE_RUN;
            }
        }
        else if (event_registry_get_app_event(context.waited_app_event.type, context.waited_app_event_time, app_event))
        {
            if ((context.waited_app_event.type != EVENT_TYPE::EVENT_APP_TIMER) || ((context.waited_app_event.type == EVENT_TYPE::EVENT_APP_TIMER) && (context.waited_app_event.value == app_event.value)))
            {
                Log.noticeln(F("react_engine_task: switching to RUN after event: %i"), context.waited_app_event.type);
                context.state = RE_RUN;
            }
        }
        break;
    case RE_PAUSE:
        // restoring context
        display_clear();
        context.pc = saved_context.pc;
        context.state = saved_context.state;
        context.timer = saved_context.timer;
        event_registry_enable_app_event();
        break;
    case RE_DONE:
        // do nothing, React Code has been successfully executed.
        break;
    default:
        break;
    }
}
