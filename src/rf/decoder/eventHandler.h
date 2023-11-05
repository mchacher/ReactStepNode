#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H


#include "rf/react_air_protocol.h"
#include <RF24Network.h>
#include "iHandler.h"

namespace communication
{

  class eventHandler : public iHandler
  {
  public:
    eventHandler(RF24Network network);
    virtual ~eventHandler() = default;

    virtual bool run(RFMessageType type, uint8_t *packet, uint8_t size) override;

  private:
    RF24Network _network;
  };
}

#endif