#ifndef ASYNC_COMMANDS_LIST
#define ASYNC_COMMANDS_LIST

#include "list.h" // Include the List class for managing asynchronous commands
#include "event_type.h"

// Structure to represent an asynchronous command
typedef struct
{
    bool active;                        // Indicates if the command is active
    uint8_t bytecode;                   // Command bytecode
    EVENT_TYPE event_type;              // Event type associated with the command
    void (*handler_function)(uint32_t *params); // Pointer to the handler function
    uint32_t params[2];                 // Maximum 2 parameters for REACT commands
} ASYNC_COMMANDS;

// Template class for managing a list of asynchronous commands
template <int MAX_ITEMS>
class AsyncCommandsList : public List<ASYNC_COMMANDS, MAX_ITEMS>
{
public:
    // Constructor
    AsyncCommandsList() {}

    /**
     * @brief Get an item from the list based on the event type.
     *
     * @param eventType The event type to search for.
     * @return A pointer to the found ASYNC_COMMANDS item, or nullptr if not found.
     */
    ASYNC_COMMANDS *getItem(EVENT_TYPE eventType)
    {
        for (size_t i = 0; i < this->size(); i++)
        {
            if (this->items[i].event_type == eventType)
            {
                return &this->items[i];
            }
        }
        return nullptr; // Event type not found in the list
    }

    /**
     * @brief Get an item from the list based on the command code.
     *
     * @param command_code The command code to search for.
     * @return A pointer to the found ASYNC_COMMANDS item, or nullptr if not found.
     */
    ASYNC_COMMANDS *getItem(uint8_t command_code)
    {
        for (size_t i = 0; i < this->size(); i++)
        {
            if (this->items[i].bytecode == command_code)
            {
                return &this->items[i];
            }
        }
        return nullptr; // Command code not found in the list
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
