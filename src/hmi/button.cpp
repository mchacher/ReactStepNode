#include "button.h"
#if LOCAL_COMMAND_BUTTONS == 1
#include "../reactmagic/event_queue.h"
#include "../reactmagic/event_registry.h"
#include "Arduino.h"
#include "ArduinoLog.h"
#include "hardware_config.h"
#include <JC_Button.h>

#if REACT_MESH == 1
#include "com/rf.h"
#endif

#define LONG_PRESS_TIME 1000 // Long press time threshold in milliseconds (1s)

#if (LOCAL_COMMAND_BUTTONS == 1)
Button pb_play_pause(PIN_PB_PLAY_PAUSE);
Button pb_stop(PIN_PB_STOP);
Button pb_custom(PIN_PB_CUSTOM);
#endif

Button pb_set(PIN_PB_SET);

/**
 * @brief Initialize buttons used in the mock-up.
 */
void button_setup()
{
    Log.notice(F("button: setup" CR));
#if (LOCAL_COMMAND_BUTTONS == 1)
    pb_play_pause.begin();
    pb_stop.begin();
    pb_custom.begin();
#endif
    pb_set.begin();
}

/**
 * @brief Read and handle button events.
 *
 * @param button The button to read.
 * @return The type of button event (short press, long press, or no event).
 */
ButtonEvent button_read(Button& button)
{
    static bool long_press = false;
    button.read();
    ButtonEvent be = BUTTON_NO_EVENT;
    if (button.pressedFor(LONG_PRESS_TIME))
    {
        if (long_press == false)
        {
            be = BUTTON_LONG_PRESS;
        }
        long_press = true;
        Log.verboseln("button: long press detected");
    }
    else if (button.wasReleased())
    {
        if (long_press)
        {
            long_press = false;
        }
        else
        {
            Log.verboseln("button: short press detected");
            be = BUTTON_PRESS;
        }
    }
    return be;
}

/**
 * @brief Perform button tasks, including event management.
 */
void button_task()
{
#if (LOCAL_COMMAND_BUTTONS == 1)
    if (button_read(pb_play_pause) != BUTTON_NO_EVENT)
    {
        event_registry_push(EVENT_SYS_TYPE_START);
        Log.noticeln(F("button task: PLAY PAUSE button pressed"));
#if REACT_MESH == 1
        // Todo: need to manage correctly the comm header
        PACKET_EVENT eventPacket;
        eventPacket.event = EVENT_SYS_TYPE_START;

        comm.masterSend(&eventPacket, SERIAL_MSG_TYPE_EVENT);
#endif
    }

    if (button_read(pb_stop) != BUTTON_NO_EVENT)
    {
        event_registry_push(EVENT_SYS_TYPE_STOP);
        Log.noticeln(F("button task: STOP button pressed"));
    }
    if (button_read(pb_custom) != BUTTON_NO_EVENT)
    {
        Log.noticeln(F("button task: CUSTOM button pressed"));
    }
#endif
    ButtonEvent e = button_read(pb_set);
    if (e == BUTTON_LONG_PRESS)
    {
        event_registry_push(EVENT_SYS_TYPE_SET_LP);
        Log.noticeln(F("button task: SET_LP button long pressed"));
    }
    else if (e == BUTTON_PRESS)
    {
        event_registry_push(EVENT_SYS_TYPE_SET_SP);
        Log.noticeln(F("button task: SET_SP button short pressed"));
    }
}
#endif
