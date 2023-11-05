#include "hardware_config.h"
#include <Arduino.h>
#include "ArduinoLog.h"
#include "eventHandler.h"
#include "react/event_registry.h"
#include "react/event_type.h"

namespace communication
{

  eventHandler::eventHandler(RF24Network network) : _network(network)
  {
  }

  bool eventHandler::run(RFMessageType type, uint8_t *packet, uint8_t size)
  {
    bool isSuccess(false);

    if (RFMessageType::REACT_EVENT == type)
    {
      EVENT event;
      memcpy(&event, packet, sizeof(EVENT));
      Log.noticeln(F("[%s] Receive event [%i]"), __func__, event.type);
      
      // check that it is a system event since only system event should be sent on RF
      // TODO: double confirm with use cases, that only system event will be sent on RF. Maybe a node can send applicative event to another node.
      if (event.type >= EVENT_SYS_TYPE_START && event.type < EVENT_SYS_TYPE_RESET)
      {
        event.timestamp = millis(); // timestamp the event
        isSuccess = event_registry_push_sys_event(event);
      }
    }
    return isSuccess;
  }

}