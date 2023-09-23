#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "Arduino.h"
#include "queue.h" // Include the original Queue class

// Define the maximum size for the event queue
#define MAX_EVENT_QUEUE_SIZE 20

// Create a new class EventQueue that inherits from the Queue class
class EventQueue : public Queue<EVENT, MAX_EVENT_QUEUE_SIZE>
{
public:
    EventQueue() : Queue<EVENT, MAX_EVENT_QUEUE_SIZE>() {}

    // Add a method to clean up the queue by removing events older than a threshold
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
                    Log.noticeln("Cleaning up the event queue: Removing an event");
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

    bool searchEvent(EVENT_TYPE EVENT_TYPE, unsigned long freshness, EVENT &resultEvent)
    {
        // Iterate through the event queue
        for (int i = front; i <= rear; i++)
        {
            EVENT currentEvent = queue[i];

            // Check if the current event matches the desired event type
            if (currentEvent.type == EVENT_TYPE)
            {
                Log.noticeln("event queue: app event timestamp = %lu", currentEvent.timestamp);
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

;

#endif // EVENT_QUEUE_H
