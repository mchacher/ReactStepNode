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

bool eventHandler::run(SERIAL_MSG_TYPE type, uint8_t *packet, uint8_t size)
{
  bool isSuccess(false);

  if (SERIAL_MSG_TYPE_EVENT == type)
  {
    PACKET_EVENT *packet_event = (PACKET_EVENT *)packet;
    Log.noticeln(F("[%s] Receive event [%i]"), __func__, packet_event->event.type);
    switch (static_cast<EVENT_TYPE>(packet_event->event.type))
    {
    case EVENT_SYS_TYPE_START:
      event_registry_push(EVENT_SYS_TYPE_START);
      Log.noticeln(F("[%s] Send internal Start event"), __func__, packet_event->event);
      isSuccess = true;
      break;
    default:
      break;
    }
  }

    // RF24NetworkHeader header;
    // uint8_t packet[256];
    // mNetwork.read(header, &packet);
    // for (int i = 0; i < sizeof(PACKET_EVENT); i++)
    // {
    //   Log.noticeln("%i", packet[i]);
    // }
    for (int i = 0; i < size; i++)
    {
      Log.noticeln("%i", packet[i]);
    }

  return isSuccess;
}

}