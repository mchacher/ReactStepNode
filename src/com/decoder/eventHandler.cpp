#include "hardware_config.h"
#include <Arduino.h>
#include "ArduinoLog.h"
#include "eventHandler.h"
#include "../../reactmagic/event_registry.h"
#include "../../reactmagic/event_type.h"

namespace communication
{

eventHandler::eventHandler(RF24Network network) :
  mNetwork(network)
{
}

bool eventHandler::run(SERIAL_MSG_TYPE type, uint8_t* packet, uint8_t size)
{
  bool isSuccess(true);

  if (SERIAL_MSG_TYPE_EVENT == type)
  {
    PACKET_EVENT* packet_event = (PACKET_EVENT*)packet;
    Log.noticeln(F("[%s] Receive event [%i]"), __func__, packet_event->event.type);
    switch (static_cast<EVENT_TYPE>(packet_event->event.type))
    {
      // System Events
    case EVENT_SYS_TYPE_START:
      event_registry_push(EVENT_SYS_TYPE_START);
      Log.noticeln(F("[%s] Send internal Start event"), __func__, packet_event->event);
      break;
    case EVENT_SYS_TYPE_STOP:
      event_registry_push(EVENT_SYS_TYPE_STOP);
      Log.noticeln(F("[%s] Send internal Stop event"), __func__, packet_event->event);
      break;

      // Application Events
    case EVENT_APP_TYPE_FOOT_PRESS:
      event_registry_push(EVENT_APP_TYPE_FOOT_PRESS);
      Log.noticeln(F("[%s] Send internal Foot press event"), __func__, packet_event->event);
      break;
    case EVENT_APP_TYPE_FOOT_PRESS_LEFT:
      event_registry_push(EVENT_APP_TYPE_FOOT_PRESS_LEFT);
      Log.noticeln(F("[%s] Send internal Foot press left event"), __func__, packet_event->event);
      break;
    case EVENT_APP_TYPE_FOOT_PRESS_RIGHT:
      event_registry_push(EVENT_APP_TYPE_FOOT_PRESS_RIGHT);
      Log.noticeln(F("[%s] Send internal Foot press right event"), __func__, packet_event->event);
      break;
    case EVENT_APP_TYPE_FOOT_RELEASE:
      event_registry_push(EVENT_APP_TYPE_FOOT_RELEASE);
      Log.noticeln(F("[%s] Send internal Foot press event"), __func__, packet_event->event);
      break;
    case EVENT_APP_TYPE_FOOT_RELEASE_LEFT:
      event_registry_push(EVENT_APP_TYPE_FOOT_RELEASE_LEFT);
      Log.noticeln(F("[%s] Send internal Foot press left event"), __func__, packet_event->event);
      break;
    case EVENT_APP_TYPE_FOOT_RELEASE_RIGHT:
      event_registry_push(EVENT_APP_TYPE_FOOT_RELEASE_RIGHT);
      Log.noticeln(F("[%s] Send internal Foot press right event"), __func__, packet_event->event);
      break;
    case EVENT_APP_TIMER:
      event_registry_push(EVENT_APP_TIMER);
      Log.noticeln(F("[%s] Send internal Timer event"), __func__, packet_event->event);
      break;
    default:
      isSuccess = false;
      break;
    }
  }
  return isSuccess;
}

}