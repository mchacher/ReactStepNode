#ifndef EVENT_DECODER_H
#define EVENT_DECODER_H

#include "com/message_format.h"
#include <RF24.h>
#include <RF24Mesh.h>
#include <RF24Network.h>
#include "IDecoder.h"

namespace communication {

class eventDecoder : public IDecoder {
public:
  eventDecoder();
  virtual ~eventDecoder() = default;

  virtual void run(SERIAL_MSG_TYPE type, const void* data) override;

private:
};
}

#endif