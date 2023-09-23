#include "Arduino.h"
#include "foot_sensor.h"
#include <ArduinoLog.h>
#include "event_registry.h"
#include "hardware_config.h"

#ifdef MINI_STEP_MOCK_UP

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
}

#endif // MINI_STEP_MOCK_UP

/**
 * @brief Check if a foot sensor is pressed.
 *
 * @param sensor A reference to the foot sensor to check.
 * @return true if the foot sensor was pressed, false otherwise.
 */
bool foot_sensor_pressed(Button& sensor)
{
    sensor.read();
    return sensor.wasPressed();
}

/**
 * @brief Perform foot sensor tasks, including event management.
 */
void foot_sensor_task()
{
    // Counter for pending left and right foot press events.
    static uint8_t pending_event_counter_left = 0;
    static uint8_t pending_event_counter_right = 0;
    
    if (foot_sensor_pressed(foot_sensor_left))
    {
        Log.noticeln(F("foot_sensor_task: FOOT_PRESS_LEFT detected"));
        // Increment the counter for pending left foot press events.
        pending_event_counter_left++;
    }
    
    if (foot_sensor_pressed(foot_sensor_right))
    {
        Log.noticeln(F("foot_sensor_task: FOOT_PRESS_RIGTH detected"));
        // Increment the counter for pending right foot press events.
        pending_event_counter_right++;
    }
    
    if (pending_event_counter_left > 0)
    {
        // Attempt to push pending left foot press events to the registry.
        if (event_registry_push(EVENT_APP_TYPE_FOOT_PRESS_LEFT))
        {
            // Decrement the counter when an event is successfully pushed.
            pending_event_counter_left--;
        }
        else 
        {
            Log.noticeln(F("foot_sensor_task: pending FOOT_PRESS_LEFT event %i"), pending_event_counter_left);
        }
    }
    
    if (pending_event_counter_right > 0)
    {
        // Attempt to push pending right foot press events to the registry.
        if (event_registry_push(EVENT_APP_TYPE_FOOT_PRESS_RIGHT))
        {
            // Decrement the counter when an event is successfully pushed.
            pending_event_counter_right--;
        }
        else 
        {
            Log.noticeln(F("foot_sensor_task: pending FOOT_PRESS_RIGHT event %i"), pending_event_counter_right);
        }
    }
}
