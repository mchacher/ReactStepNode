#include "Arduino.h"
#include "foot_sensor.h"
#include <ArduinoLog.h>
#include "../reactmagic/event_registry.h"
#include "../hardware_config.h"
#include "../app_config.h"

#if (DIGITAL_FOOT_SENSOR == 1)

#include <JC_Button.h>

Button foot_sensor_left(PIN_PB_FOOT_LEFT);
Button foot_sensor_right(PIN_PB_FOOT_RIGHT);

/**
 * @brief Initialize the foot sensors.
 */
void foot_sensor_setup()
{
    Log.notice(F("foot_sensor: setup" CR));
    foot_sensor_left.begin();
    foot_sensor_right.begin();
    delay(50);// to avoid first false event
    foot_sensor_left.read();
    foot_sensor_right.read();
}


enum FootSensorState {
    FOOT_SENSOR_NONE,
    FOOT_SENSOR_PRESSED,
    FOOT_SENSOR_RELEASED
};



/**
 * @brief Check the foot sensor state (pressed or released).
 *
 * @param sensor A reference to the foot sensor to check.
 * @return The state of the foot sensor (FOOT_SENSOR_PRESSED, FOOT_SENSOR_RELEASED, or FOOT_SENSOR_NONE).
 */
FootSensorState checkFootSensorState(Button& sensor)
{
    sensor.read();

    if (sensor.wasPressed())
    {
        Log.noticeln(F("foot_sensor_task: FOOT_SENSOR_PRESSED"));
        return FOOT_SENSOR_PRESSED;
    }

    if (sensor.wasReleased())
    {
        Log.noticeln(F("foot_sensor_task: FOOT_SENSOR_RELEASED"));
        return FOOT_SENSOR_RELEASED;
    }

    return FOOT_SENSOR_NONE;
}

/**
 * @brief Perform foot sensor tasks, including event management.
 */
void foot_sensor_task()
{
    // Counter for pending left and right foot press and release events.
    static uint8_t pending_press_left = 0;
    static uint8_t pending_release_left = 0;
    static uint8_t pending_press_right = 0;
    static uint8_t pending_release_right = 0;

    // Check the left foot sensor state
    FootSensorState leftState = checkFootSensorState(foot_sensor_left);

    if (leftState == FOOT_SENSOR_PRESSED)
    {
        // Increment the counter for pending left foot press events.
        pending_press_left++;
    }
    else if (leftState == FOOT_SENSOR_RELEASED)
    {
        // Increment the counter for pending left foot release events.
        pending_release_left++;
    }

    // Check the right foot sensor state
    FootSensorState rightState = checkFootSensorState(foot_sensor_right);

    if (rightState == FOOT_SENSOR_PRESSED)
    {
        // Increment the counter for pending right foot press events.
        pending_press_right++;
    }
    else if (rightState == FOOT_SENSOR_RELEASED)
    {
        // Increment the counter for pending right foot release events.
        pending_release_right++;
    }

    if (pending_press_left > 0)
    {
        // Attempt to push pending left foot press events to the registry.
        if (event_registry_push(EVENT_APP_TYPE_FOOT_PRESS_LEFT))
        {
            // Decrement the counter when an event is successfully pushed.
            pending_press_left--;
        }
        else
        {
            Log.noticeln(F("foot_sensor_task: pending FOOT_PRESS_LEFT event %i"), pending_press_left);
        }
    }

    if (pending_release_left > 0)
    {
        // Attempt to push pending left foot release events to the registry.
        if (event_registry_push(EVENT_APP_TYPE_FOOT_RELEASE_LEFT))
        {
            // Decrement the counter when an event is successfully pushed.
            pending_release_left--;
        }
        else
        {
            Log.noticeln(F("foot_sensor_task: pending FOOT_RELEASE_LEFT event %i"), pending_release_left);
        }
    }

    if (pending_press_right > 0)
    {
        // Attempt to push pending right foot press events to the registry.
        if (event_registry_push(EVENT_APP_TYPE_FOOT_PRESS_RIGHT))
        {
            // Decrement the counter when an event is successfully pushed.
            pending_press_right--;
        }
        else
        {
            Log.noticeln(F("foot_sensor_task: pending FOOT_PRESS_RIGHT event %i"), pending_press_right);
        }
    }

    if (pending_release_right > 0)
    {
        // Attempt to push pending right foot release events to the registry.
        if (event_registry_push(EVENT_APP_TYPE_FOOT_RELEASE_RIGHT))
        {
            // Decrement the counter when an event is successfully pushed.
            pending_release_right--;
        }
        else
        {
            Log.noticeln(F("foot_sensor_task: pending FOOT_RELEASE_RIGHT event %i"), pending_release_right);
        }
    }
}

#endif