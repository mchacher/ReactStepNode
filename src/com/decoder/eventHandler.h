#ifndef EVENT_DECODER_H
#define EVENT_DECODER_H

#include "../message_format.h"
#include <RF24Network.h>
#include "iHandler.h"

namespace communication
{

  class eventHandler : public iHandler
  {
  public:
    eventHandler(RF24Network network);
    virtual ~eventHandler() = default;

    virtual bool run(MSG_TYPE type, uint8_t *packet, uint8_t size) override;

  private:
    RF24Network mNetwork;
  };
}

#endif