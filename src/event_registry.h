#ifndef EVENT_REGISTRY_H
#define EVENT_REGISTRY_H

#include "Arduino.h"
#include "event_type.h"


void event_registry_setup();
void event_registry_task();
bool event_registry_is_empty();
bool event_registry_is_full();
bool event_registry_push(EVENT_TYPE_APP event_type);
bool event_registry_pop(Event_App& event);

#endif