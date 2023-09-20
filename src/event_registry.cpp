#include "Arduino.h"
#include <ArduinoLog.h>
#include "event_registry.h"
#include "event_type.h"
#include "queue.h" 

#define MAX_EVENT_QUEUE_SIZE 10

// Define the duration threshold for event removal (in milliseconds)
const uint32_t EVENT_DURATION_THRESHOLD = 2000;

// Create an instance of the Queue class for application events
Queue<EVENT_APP, MAX_EVENT_QUEUE_SIZE> queue_app_event;

/**
 * @brief Initialize the event registry (queue_app_event) when the system starts.
 */
void event_registry_setup()
{
}

/**
 * @brief Task function to clean and remove application events older than EVENT_DURATION_THRESHOLD.
 */
void event_registry_task()
{
    // Get the current time in milliseconds
    uint32_t currentMillis = millis();

    // Iterate through the application event queue
    while (!queue_app_event.isEmpty()) {
        EVENT_APP frontEvent;

        // Access the front application event without removing it
        if (queue_app_event.frontItem(frontEvent)) {
            // Check if the front event has been in the queue for more than the threshold
            if (currentMillis - frontEvent.timestamp > EVENT_DURATION_THRESHOLD) {
                Log.noticeln("event_registry_task: removing front application event");
                // Remove the application event from the queue
                queue_app_event.pop(frontEvent);
            } else {
                // The application event is within the threshold, so you can process it if needed
                // For example: Handle the event
                // handleEvent(frontEvent);
                break; // Exit the loop since further events are newer
            }
        }
    }
}

/**
 * @brief Check if the event registry (queue_app_event) is empty.
 *
 * @return true if the event registry is empty, false otherwise.
 */
bool event_registry_is_empty()
{
    return queue_app_event.isEmpty();
}

/**
 * @brief Check if the event registry (queue_app_event) is full.
 *
 * @return true if the event registry is full, false otherwise.
 */
bool event_registry_is_full()
{
    return queue_app_event.isFull();
}

/**
 * @brief Push an application event onto the event registry (queue_app_event).
 *
 * @param event_type The event type to push onto the queue.
 * @return true if the event was successfully pushed, false if the queue is full.
 */
bool event_registry_push(EVENT_TYPE_APP event_type)
{
    EVENT_APP event;
    event.type = event_type;
    event.timestamp = millis();
    
    // Push the application event onto the event queue
    return queue_app_event.push(event);
}

/**
 * @brief Pop (remove and retrieve) an application event from the event registry (queue_app_event).
 *
 * @param event The event to pop from the queue.
 * @return true if an event was successfully popped, false if the queue is empty.
 */
bool event_registry_pop(EVENT_APP &event)
{
    // Pop the application event from the event queue
    return queue_app_event.pop(event);
}
