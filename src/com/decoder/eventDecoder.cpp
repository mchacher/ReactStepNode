#include "hardware_config.h"
#include <Arduino.h>
#include "ArduinoLog.h"
#include "eventDecoder.h"
#include "reactmagic\event_registry.h"
#include "reactmagic\event_type.h"

namespace communication
{

eventDecoder::eventDecoder()
{
  // Constructor for RF class when REACT_MESH is defined
}

void eventDecoder::run(SERIAL_MSG_TYPE type, const void* rawData)
{
  if (SERIAL_MSG_TYPE_EVENT == type)
  {
    const PACKET_EVENT* data = static_cast<const PACKET_EVENT*>(rawData);
    Log.noticeln(F("[%s] Receive event [%d]"), __func__, data->event);
    switch (static_cast<EVENT_TYPE>(data->event))
    {
    case EVENT_SYS_TYPE_START:
      event_registry_push(EVENT_SYS_TYPE_START);
      Log.noticeln(F("[%s] Send internal Start event"), __func__, data->event);
      break;
    }
  }
}

}