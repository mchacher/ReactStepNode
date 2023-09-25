#include "Arduino.h"
#include "display.h"
#include <TM1637TinyDisplay.h>
#include <ArduinoLog.h>
#include "hardware_config.h"
#include "state_machine.h"

TM1637TinyDisplay display(PIN_TM_1637_CLK, PIN_TM_1637_DIO);


static const char *msg = MSG_HELLO;
static unsigned long expiration_time = 0;

void display_setup()
{
    Log.noticeln(F("display: setup"));
    display.begin();
    display.setBrightness(5);

}

void display_message(const char *message)
{
    
    display.stopAnimation();
    display.clear();
    msg = message;
    display.startStringScroll_P(msg, 300);
    expiration_time = 0; // msg will be displayed indefinitely
}


void display_message(const char *message, uint8_t duration)
{
    
    display.stopAnimation();
    display.clear();
    msg = message;
    display.startStringScroll_P(msg, 300);
    expiration_time = millis() + duration * 1000;
}


void display_task()
{
    // display.showString(msg);
    bool isAnimationRunning = display.Animate();
    
    if (!isAnimationRunning)
    {
        Log.verbose(F("display: Animation not running"));
        display.startStringScroll_P(msg, 300);
    }

    if (expiration_time != 0 && millis() > expiration_time)
    {
        display_clear();
    }
}

void display_clear()
{
    display.clear();
    msg= MSG_NONE;
}