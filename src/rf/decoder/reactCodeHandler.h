#ifndef REACT_CODE_HHANDLER_H
#define REACT_CODE_HHANDLER_H

#include "rf/react_air_protocol.h"
#include <RF24Network.h>
#include "iHandler.h"

namespace communication
{

  class reactCodeHandler : public iHandler
  {
  public:
    reactCodeHandler(RF24Network network);
    virtual ~reactCodeHandler() = default;

    virtual bool run(RFMessageType type, uint8_t *packet, uint8_t size) override;

  private:
    RF24Network _network;
  };
}

#endif