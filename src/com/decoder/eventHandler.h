#ifndef EVENT_DECODER_H
#define EVENT_DECODER_H

#include "com\message_format.h"
#include <RF24Network.h>
#include "iHandler.h"

namespace communication {

class eventHandler : public iHandler {
public:
  eventHandler(RF24Network network);
  virtual ~eventHandler() = default;

  virtual bool run(SERIAL_MSG_TYPE type) override;

private:

  RF24Network mNetwork;

};
}

#endif