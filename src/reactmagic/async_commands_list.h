#ifndef ASYNC_COMMANDS_LIST
#define ASYNC_COMMANDS_LIST

#include "../utils/list.h" // Include the List class for managing asynchronous commands
#include "event_type.h"
#include "ArduinoLog.h"

// Structure to represent an asynchronous command
typedef struct
{
    bool active;                                // Indicates if the command is active
    uint8_t bytecode;                           // Command bytecode
    EVENT_TYPE event_type;                      // Event type associated with the command
    void (*handler_function)(uint32_t *params); // Pointer to the handler function
    uint32_t params[2];                         // Maximum 2 parameters for REACT commands
} ASYNC_COMMANDS;

// Template class for managing a list of asynchronous commands
template <int MAX_ITEMS>
class AsyncCommandsList : public List<ASYNC_COMMANDS, MAX_ITEMS>
{
public:
    AsyncCommandsList() {}

    /**
     * @brief Get a list of items from the list based on the event type.
     *
     * @param eventType The event type to search for.
     * @param[out] resultItems A pointer to an array of ASYNC_COMMANDS pointers to store the results.
     * @param[in] maxResults The maximum number of results to store in the resultItems array.
     * @return The number of items found.
     */
    size_t getItemsByEventType(EVENT_TYPE eventType, ASYNC_COMMANDS *resultItems[], size_t maxResults)
    {
        size_t itemCount = 0;
        Log.verboseln(F("AsyncCommandsList size: %i"), this->size());
        for (size_t i = 0; i < this->size() && itemCount < maxResults; i++)
        {
            Log.verboseln(F("AsyncCommandsList event_type: %i"), this->items[i].event_type);
            if (this->items[i].event_type == eventType)
            {
                resultItems[itemCount] = &this->items[i];
                itemCount++;
            }
        }

        return itemCount;
    }

    /**
     * @brief Get items from the list based on the specified command code.
     *
     * @param command_code The command code to search for.
     * @param resultItems An array to store matching ASYNC_COMMANDS items.
     * @param maxResults The maximum number of matching items to return.
     * @return The number of matching items found and stored in resultItems.
     */
    size_t getItemsByCommandCode(uint8_t command_code, ASYNC_COMMANDS *resultItems[], size_t maxResults)
    {
        size_t itemCount = 0;
        Log.verboseln(F("AsyncCommandsList size: %i"), this->size());
        for (size_t i = 0; i < this->size() && itemCount < maxResults; i++)
        {
            Log.verboseln(F("AsyncCommandsList command_code: %i"), this->items[i].bytecode);
            if (this->items[i].bytecode == command_code)
            {
                resultItems[itemCount] = &this->items[i];
                itemCount++;
            }
        }

        return itemCount;
    }

    /**
     * @brief Get an item from the list based on the command code and event type.
     *
     * @param command_code The command code to search for.
     * @param eventType The event type to search for.
     * @return A pointer to the found ASYNC_COMMANDS item, or nullptr if not found.
     */
    ASYNC_COMMANDS *getItem(uint8_t command_code, EVENT_TYPE eventType)
    {
        for (size_t i = 0; i < this->size(); i++)
        {
            if ((this->items[i].bytecode == command_code) && (this->items[i].event_type == eventType))
            {
                return &this->items[i];
            }
        }
        return nullptr; // Command code and event type not found in the list
    }
};

#endif
