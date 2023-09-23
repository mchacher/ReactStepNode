
#include <Arduino.h>
#include <ArduinoLog.h>
#include "react_engine.h"
#include "led.h"
#include "event_type.h"
#include "event_registry.h"

// Command codes
#define START 0x80
#define END 0x81
#define WAIT_EVENT 0x82
#define TIMER 0x83
#define SEND_EVENT 0x84
#define LED_COLOR 0xA0
#define LED_TRAFFIC_LIGHT 0xA1
#define FOOT_PRESS_COLOR 0xB0
#define FOOT_PRESS_BIP 0xB1

// Argument codes
#define COLOR_WHITE 0xFFFFFF
#define COLOR_BLACK 0x000000
#define COLOR_RED 0xFF0000
#define COLOR_GREEN 0x008000
#define COLOR_BLUE 0x0000FF
#define COLOR_PINK 0xFFC0CB
#define COLOR_ORANGE 0xFFA500
#define COLOR_YELLOW 0xFFFF00

//     0     1     2     3     4     5
/** 0x80, 0xA0, 0xFF, 0x00, 0x00, 0x83, 0x00, 0x05, 0xA0, */
uint8_t exampleReactCode[] = {
    START,
    LED_COLOR, 0xFF, 0x00, 0x00,                                                       // LED ROUGE
    TIMER, 0, 2,                                                                       // Timer 2s
    LED_COLOR, (COLOR_BLUE >> 16) & 0xFF, (COLOR_BLUE >> 8) & 0xFF, COLOR_BLUE & 0xFF, // LED BLEUE
    WAIT_EVENT, EVENT_APP_TYPE_FOOT_PRESS_LEFT,                                             // WAIT EVENT FOOT PRESS
    END};

static int pc = 0;
static uint16_t timer = 0;
static uint8_t waited_app_event = 0;
static unsigned long waited_app_event_time;
static RE_STATE re_state = RE_IDLE;
static uint16_t tick = 0;

void handle_start()
{
    // Add your custom code for the START command here
}

void handle_end()
{
    // Add your custom code for the END command here
}

void handle_wait_event(uint32_t eventCode)
{
    // Add your custom code for the WAIT_EVENT command here
}

void handle_timer(uint16_t duration)
{
    // Add your custom code for the TIMER command here
}

void handle_send_event()
{
    // Add your custom code for the SEND command here
}

void handle_led_color(uint32_t colorCode)
{
    // Add your custom code for the LED_COLOR command here
}

void handle_led_traffic_light()
{
    // Add your custom code for the LED_TRAFFIC_LIGHT command here
}

void handle_foot_press_color(uint32_t color1, uint32_t color2)
{
    // Add your custom code for the FOOT_PRESS_COLOR command here
}

void handle_foot_press_bip()
{
    // Add your custom code for the FOOT_PRESS_BIP command here
}

//     0     1     2     3     4     5     6     7     8
/** 0x80, 0xA0, 0xFF, 0x00, 0x00, 0x83, 0x00, 0x05, 0xA0, */

// Function to interpret and execute commands with arguments
void interpret_command(uint8_t *bytecode)
{
    uint8_t command = bytecode[pc];
    pc = pc + 1;
    switch (command)
    {
    case START:
        // Handle START command
        Log.noticeln(F("react_engine: Command START"));
        break;
    case END:
        // Handle END command
        Log.noticeln(F("react_engine: Command END"));
        pc = 0;
        break;
    case WAIT_EVENT:
        re_state = RE_HOLD_WAIT_APP_EVENT;
        waited_app_event = bytecode[pc];
        waited_app_event_time = millis();
        pc = pc + 1;
        Log.noticeln(F("react_engine: Command WAIT_EVENT, event: %i"), waited_app_event);
        event_registry_enable_app_event();
        break;
    case TIMER:
        // Handle TIMER command with args
        Log.noticeln(F("react_engine: Command TIMER %l"), millis());
        timer = (uint16_t)(bytecode[pc] << 8);
        timer |= bytecode[pc + 1];
        pc = pc + 2;
        re_state = RE_HOLD_TIMER_ENABLE;
        break;
    case SEND_EVENT:
        Log.noticeln(F("react_engine: Command SEND_EVENT"));
        // Handle SEND command
        break;
    case LED_COLOR:
        Log.noticeln(F("react_engine: Command LED_COLOR"));
        uint32_t color = ((uint32_t)bytecode[pc] << 16);
        color |= ((uint32_t)bytecode[pc + 1] << 8);
        color |= bytecode[pc + 2];
        pc = pc + 3;
        led_set_color(color);
        // Handle LED_COLOR command with args
        break;
    case LED_TRAFFIC_LIGHT:
        Log.noticeln(F("react_engine: Command LED_TRAFFIC_LIGHT"));
        // Handle LED_TRAFFIC_LIGHT command
        break;
    case FOOT_PRESS_COLOR:
        Log.notice(F("react_engine: Command FOOT_PRESS_COLOR"));
        // Handle FOOT_PRESS_COLOR command with args
        break;
    case FOOT_PRESS_BIP:
        Log.notice(F("react_engine: Command FOOT_PRESS_BIP"));
        // Handle FOOT_PRESS_BIP command
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

void react_engine_task()
{
    switch (re_state)
    {
    case RE_READY:
        re_state = RE_RUN;
    case RE_RUN:
        tick = 0;
        interpret_command(exampleReactCode);
        break;
    case RE_HOLD_TIMER_ENABLE:
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
        // if (event_registry_pop_app_event(app_event))
        // {
        //     // An application event was successfully popped.
        //     if ((app_event.type == waited_app_event) || ((waited_app_event == EVENT_APP_TYPE_FOOT_PRESS) && ((app_event.type == EVENT_APP_TYPE_FOOT_PRESS_LEFT) || (app_event.type == EVENT_APP_TYPE_FOOT_PRESS_RIGHT))))
        //     {
        //         re_state = RE_RUN;
        //     }
        // }
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
    }
}