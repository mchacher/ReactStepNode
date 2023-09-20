#include "Arduino.h"
#include "foot_sensor.h"
#include <ArduinoLog.h>
#include "event_registry.h"

#ifdef MOCKUP
#include <JC_Button.h> 

#define PIN_BUTTON 5

Button foot_sensor(PIN_BUTTON);   

void foot_sensor_setup()
{
    foot_sensor.begin();
}

#endif

bool foot_sensor_pressed()
{
    foot_sensor.read();
    return foot_sensor.wasPressed();
}

void foot_sensor_task()
{
    static bool pending_event = false;
    static bool status = false;
    status = foot_sensor_pressed();
    if (status || pending_event)
    {
        Log.noticeln(F("------ pushing FOOT_PRESS event"));
        // To push a workout event (e.g., foot press):
        pending_event = !event_registry_push(EVENT_APP_TYPE_FOOT_PRESS);
    }
}