#include "Arduino.h"
#include "button.h"
#include "ArduinoLog.h"
#include <JC_Button.h>
#include "hardware_config.h"
#include "event_type.h"
#include "event_registry.h"

#ifdef MINI_STEP_MOCK_UP

Button pb_play_pause(PIN_PB_PLAY_PAUSE);
Button pb_stop(PIN_PB_STOP);
Button pb_set(PIN_PB_SET);
Button pb_custom(PIN_PB_CUSTOM);

void button_setup()
{
    Log.notice(F("button: setup" CR));
    pb_play_pause.begin();
    pb_stop.begin();
    pb_set.begin();
    pb_custom.begin();
}

#endif

bool is_button_pressed(Button &button)
{
    button.read();
    return button.wasPressed();
}

void button_task()
{
    if (is_button_pressed(pb_play_pause))
    {
        event_registry_push(EVENT_SYS_TYPE_START);
        Log.noticeln(F("button task: PLAY PAUSE button pressed"));
    }

    if (is_button_pressed(pb_stop))
    {
        event_registry_push(EVENT_SYS_TYPE_STOP);
        Log.noticeln(F("button task: STOP button pressed"));
    }

    if (is_button_pressed(pb_set))
    {
        event_registry_push(EVENT_SYS_TYPE_SET);
        Log.noticeln(F("button task: SET button pressed"));
    }

    if (is_button_pressed(pb_custom))
    {
        Log.noticeln(F("button task: CUSTOM button pressed"));
    }
}
