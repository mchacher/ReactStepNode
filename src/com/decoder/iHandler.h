#ifndef I_DECODER_H
#define I_DECODER_H

#include "../message_format.h"

namespace communication {

class iHandler {
public:
  iHandler() = default;
  virtual ~iHandler() = default;

  virtual bool run(MSG_TYPE type, uint8_t *packet, uint8_t size) = 0;

};


}

#endif