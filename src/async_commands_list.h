#ifndef ASYNS_COMMANDS_LIST
#define ASYNS_COMMANDS_LIST

#include "list.h"
#include "event_type.h"

typedef struct 
{
    bool active;
    uint8_t bytecode;
    EVENT_TYPE event_type;
    void (*handler_function)();
} ASYNC_COMMANDS;

template <int MAX_ITEMS>
class AsyncCommandsList : public List<ASYNC_COMMANDS, MAX_ITEMS> {
public:
    // Constructor
    AsyncCommandsList() {}

    // Add this method to your AsyncCommandsList class
    ASYNC_COMMANDS* getItemByEventType(EVENT_TYPE eventType) {
        for (size_t i = 0; i < this->size(); i++) {
            if (this->items[i].event_type == eventType) {
                return &this->items[i];
            }
        }
        return nullptr; // Event type not found in the list
    }
};

#endif
