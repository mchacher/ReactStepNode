#include "Arduino.h"
#include "display.h"
#include <TM1637.h>
#include <ArduinoLog.h>
#include "hardware_config.h"

// Instantiation and pins configurations
TM1637 tm(PIN_TM_1637_CLK, PIN_TM_1637_DIO);

void display_setup()
{
    Log.noticeln(F("display: setup"));
    tm.begin();
    tm.setBrightness(5);
    tm.display("HELLO");
}


void display_task()
{
    // Nothing
}