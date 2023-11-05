#include "hardware_config.h"
#include <Arduino.h>
#include "ArduinoLog.h"
#include "reactCodeHandler.h"
#include "react/event_registry.h"
#include "react/event_type.h"
#include "react/react_code.h"
#include "react/react_engine.h"
#include "ArduinoLog.h"
#include "rf/react_air_protocol.h"

namespace communication
{

  reactCodeHandler::reactCodeHandler(RF24Network network) : _network(network)
  {
  }

  bool reactCodeHandler::run(RFMessageType type, uint8_t *packet, uint8_t size)
  {
    bool isSuccess(false);
    Log.noticeln("reactCodeHandler: analysing if message is a react code");
    if (RFMessageType::REACT_CODE == type)
    {
        Log.noticeln("reactCodeHandler:[%s]: this is a react code message");
        // copy code to react code
        react_engine_save_react_code(packet);
        // double buffer or depending on statemachine ?
        isSuccess = true;
    }
    return isSuccess;
  }

}