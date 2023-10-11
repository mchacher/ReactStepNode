#include "hardware_config.h"
#include <Arduino.h>
#include "ArduinoLog.h"
#include "eventHandler.h"
#include "reactmagic\event_registry.h"
#include "reactmagic\event_type.h"

namespace communication
{

eventHandler::eventHandler(RF24Network network) :
  mNetwork(network)
{
}

bool eventHandler::run(SERIAL_MSG_TYPE type)
{
  bool isSuccess(false);

  if (SERIAL_MSG_TYPE_EVENT == type)
  {
    RF24NetworkHeader header;
    PACKET_EVENT data;
    mNetwork.read(header, &data);
    Log.noticeln(F("[%s] Receive event [%d]"), __func__, data.event);

    switch (static_cast<EVENT_TYPE>(data.event))
    {
    case EVENT_SYS_TYPE_START:
      event_registry_push(EVENT_SYS_TYPE_START);
      Log.noticeln(F("[%s] Send internal Start event"), __func__, data.event);
      isSuccess = true;
      break;
    default:
      break;
    }
  }

  return isSuccess;
}

}