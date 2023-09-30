#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "Arduino.h"
#include "queue.h" // Include the original Queue class
#include "event_type.h"
#include "ArduinoLog.h"

// Define the maximum size for the event queue
#define MAX_EVENT_QUEUE_SIZE 8

// Create a new class EventQueue that inherits from the Queue class
class EventQueue : public Queue<EVENT, MAX_EVENT_QUEUE_SIZE>
{
public:
    EventQueue() : Queue<EVENT, MAX_EVENT_QUEUE_SIZE>() {}

    /**
     * @brief Clean up the event queue by removing events older than a threshold.
     *
     * @param currentMillis The current value of millis() for time comparison.
     * @param EVENT_DURATION_THRESHOLD The threshold duration in milliseconds for event removal.
     */
    void cleanUpQueue(uint32_t currentMillis, uint32_t EVENT_DURATION_THRESHOLD)
    {
        // Iterate through the event queue
        while (!isEmpty())
        {
            EVENT frontEvent;

            // Access the front event without removing it
            if (frontItem(frontEvent))
            {
                // Check if the front event has been in the queue for more than the threshold
                if (currentMillis - frontEvent.timestamp > EVENT_DURATION_THRESHOLD)
                {
                    Log.noticeln(F("event queue: removing front event - time has expired"));
                    // Remove the event from the queue
                    pop(frontEvent);
                }
                else
                {
                    // The event is within the threshold, so you can process it if needed
                    // For example: Handle the event
                    // handleEvent(frontEvent);
                    break; // Exit the loop since further events are newer
                }
            }
        }
    }

    /**
     * @brief Search for a specific event in the queue with a certain freshness threshold.
     *
     * @param EVENT_TYPE The type of event to search for.
     * @param freshness The freshness threshold in milliseconds.
     * @param resultEvent [out] The matching event found (if any).
     * @return true if a matching event with sufficient freshness is found, false otherwise.
     */
    bool searchEvent(EVENT_TYPE EVENT_TYPE, unsigned long freshness, EVENT &resultEvent)
    {
        // Iterate through the event queue
        for (int i = front; i <= rear; i++)
        {
            EVENT currentEvent = queue[i];

            // Check if the current event matches the desired event type
            if (currentEvent.type == EVENT_TYPE)
            {
                Log.verbose(F("event queue: app event timestamp = %lu"), currentEvent.timestamp);
                // Check if the current event has freshness higher than the threshold
                if (currentEvent.timestamp > freshness)
                {
                    // Found a matching event with sufficient freshness
                    resultEvent = currentEvent;

                    // Remove the event from the queue
                    for (int j = i; j < rear; j++)
                    {
                        queue[j] = queue[j + 1];
                    }
                    rear--;

                    return true;
                }
            }
        }

        // No matching event found
        return false;
    }
};

#endif // EVENT_QUEUE_H
