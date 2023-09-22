
#include <Arduino.h>
#include <ArduinoLog.h>
#include "runtime.h"
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
    LED_COLOR, 0xFF, 0x00, 0x00, // LED ROUGE
    TIMER, 0, 2, // Timer 2s
    LED_COLOR, (COLOR_BLUE >> 16) & 0xFF, (COLOR_BLUE >> 8) & 0xFF, COLOR_BLUE & 0xFF, // LED BLEUE
    WAIT_EVENT, EVENT_APP_TYPE_FOOT_PRESS, // WAIT EVENT FOOT PRESS
    END};

int pc = 0;
uint16_t timer = 0;
uint8_t event_app = 0;
RT_State state = RT_IDLE;

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
        Log.noticeln(F("------ Command START"));
        break;
    case END:
        // Handle END command
        Log.noticeln(F("------ Command END"));
        pc = 0;
        break;
    case WAIT_EVENT:
        state = RT_HOLD_WAIT_APP_EVENT;
        event_app = bytecode[pc];
        pc = pc + 1;
        Log.noticeln(F("------ Command WAIT_EVENT, event: %i"), event_app);
        break;
    case TIMER:
        // Handle TIMER command with args
        Log.noticeln(F("------ Command TIMER %l"), millis());
        timer = (uint16_t)(bytecode[pc] << 8);
        timer |= bytecode[pc + 1];
        pc = pc + 2;
        state = RT_HOLD_TIMER_ENABLE;
        break;
    case SEND_EVENT:
        Log.noticeln(F("------ Command SEND_EVENT"));
        // Handle SEND command
        break;
    case LED_COLOR:
        Log.noticeln(F("------ Command LED_COLOR"));
        uint32_t color = ((uint32_t)bytecode[pc] << 16);
        color |= ((uint32_t)bytecode[pc + 1] << 8);
        color |= bytecode[pc + 2];
        pc = pc + 3;
        led_set_color(color);
        // Handle LED_COLOR command with args
        break;
    case LED_TRAFFIC_LIGHT:
        Log.noticeln(F("------ Command LED_TRAFFIC_LIGHT"));
        // Handle LED_TRAFFIC_LIGHT command
        break;
    case FOOT_PRESS_COLOR:
        Log.notice(F("------ Command FOOT_PRESS_COLOR"));
        // Handle FOOT_PRESS_COLOR command with args
        break;
    case FOOT_PRESS_BIP:
        Log.notice(F("------ Command FOOT_PRESS_BIP"));
        // Handle FOOT_PRESS_BIP command
        break;
    default:
        Log.noticeln(F("------ Command UNKNOWN COMMAND"));
        // Unknown command
        break;
    }
}

void runtime_setup()
{
    state = RT_READY;
}

void runtime_task()
{
    static uint16_t tick = 0;
    // check whether there is a system event
    // TODO ...
    switch (state)
    {
    case RT_READY:
        event_registry_enable_app_event();
        state = RT_RUN;
    case RT_RUN:
        tick = 0;
        interpret_command(exampleReactCode);
        break;
    case RT_HOLD_TIMER_ENABLE:
        tick++;
        if ((tick * RUNTIME_CYCLE) == 1000) // 1s
        {
            timer = timer - 1;
            if (timer == 0)
            {
                state = RT_RUN;
            }
            tick = 0;
        }
        break;
    case RT_HOLD_WAIT_APP_EVENT:
        EVENT app_event;
        if (event_registry_pop_app_event(app_event))
        {
            // An application event was successfully popped.
            if (app_event.type == event_app)
            {
                state = RT_RUN;
            }
        }
        break;
    }
}