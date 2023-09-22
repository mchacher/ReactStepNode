#include "Arduino.h"
#include <ArduinoLog.h>
#include "event_registry.h"
#include "event_type.h"
#include "event_queue.h" // Include the EventQueue class

// Create an instance of the EventQueue class for application events
EventQueue queue_app_event;
// Create an instance of the EventQueue class for system events
EventQueue queue_sys_event;

// Define the duration threshold for event removal (in milliseconds)
const uint32_t EVENT_DURATION_THRESHOLD = 2000;

bool app_event = false;

/**
 * @brief Initialize the event registry (queue_app_event and queue_sys_event) when the system starts.
 */
void event_registry_setup()
{
    // No need to initialize the queues here; it's done in the EventQueue constructor.
}

/**
 * @brief Task function to clean and remove application events and system events older than EVENT_DURATION_THRESHOLD.
 */
void event_registry_task()
{
    // Get the current time in milliseconds
    uint32_t currentMillis = millis();

    // Clean up the application event queue
    queue_app_event.cleanUpQueue(currentMillis, EVENT_DURATION_THRESHOLD);

    // Clean up the system event queue
    queue_sys_event.cleanUpQueue(currentMillis, EVENT_DURATION_THRESHOLD);
}

/**
 * @brief Check if the application event registry (queue_app_event) is empty.
 *
 * @return true if the application event registry is empty, false otherwise.
 */
bool event_registry_is_empty_app_event()
{
    return queue_app_event.isEmpty();
}

/**
 * @brief Check if the system event registry (queue_sys_event) is empty.
 *
 * @return true if the system event registry is empty, false otherwise.
 */
bool event_registry_is_empty_sys_event()
{
    return queue_sys_event.isEmpty();
}

/**
 * @brief Check if the application event registry (queue_app_event) is full.
 *
 * @return true if the application event registry is full, false otherwise.
 */
bool event_registry_is_full_app_event()
{
    return queue_app_event.isFull();
}

/**
 * @brief Check if the system event registry (queue_sys_event) is full.
 *
 * @return true if the system event registry is full, false otherwise.
 */
bool event_registry_is_full_sys_event()
{
    return queue_sys_event.isFull();
}

/**
 * @brief Push an event onto the appropriate event registry (queue_app_event or queue_sys_event)
 *        based on the event type.
 *
 * @param event_type The event type to push onto the queue.
 * @return true if the event was successfully pushed, false if the queue is full.
 */
bool event_registry_push(EVENT_TYPE event_type)
{
    EVENT event;
    event.type = event_type;
    event.timestamp = millis();

    // Determine which queue to push the event to based on its type
    if (event_type >= EVENT_SYS_TYPE_START && event_type < EVENT_SYS_TYPE_RESET)
    {
        // Push the system event onto the system event queue
        return queue_sys_event.push(event);
    }
    else if (app_event && (event_type >= EVENT_APP_TYPE_FOOT_PRESS) && (event_type < 0x80))
    {
        // Push the application event onto the application event queue
        return queue_app_event.push(event);
    }
    else
    {
        // Invalid event type
        return false;
    }
}

/**
 * @brief Pop (remove and retrieve) an application event from the application event registry (queue_app_event).
 *
 * @param event The event to pop from the queue.
 * @return true if an event was successfully popped, false if the queue is empty.
 */
bool event_registry_pop_app_event(EVENT &event)
{
    // Pop the application event from the application event queue
    return queue_app_event.pop(event);
}

/**
 * @brief Pop (remove and retrieve) a system event from the system event registry (queue_sys_event).
 *
 * @param event The event to pop from the queue.
 * @return true if an event was successfully popped, false if the queue is empty.
 */
bool event_registry_pop_sys_event(EVENT &event)
{
    // Pop the system event from the system event queue
    return queue_sys_event.pop(event);
}

/**
 * @brief Disable App Events. Application events are not managed by the registry.
 *
 */
void event_registry_disable_app_event()
{
    app_event = false;
    queue_app_event.flush();
}

/**
 * @brief Enable App Events. Application events are managed by the registry.
 *
 */
void event_registry_enable_app_event()
{
    app_event = true;
}