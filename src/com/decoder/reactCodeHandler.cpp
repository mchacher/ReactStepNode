#include "hardware_config.h"
#include <Arduino.h>
#include "ArduinoLog.h"
#include "reactCodeHandler.h"
#include "../../reactmagic/event_registry.h"
#include "../../reactmagic/event_type.h"
#include "../../reactmagic/react_code.h"
#include "../../reactmagic/react_engine.h"
#include "ArduinoLog.h"

namespace communication
{

  reactCodeHandler::reactCodeHandler(RF24Network network) : mNetwork(network)
  {
  }

  bool reactCodeHandler::run(MSG_TYPE type, uint8_t *packet, uint8_t size)
  {
    bool isSuccess(false);
    Log.noticeln("reactCodeHandler: analysing if message is a react code");
    if (MSG_TYPE_REACT_CODE == type)
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