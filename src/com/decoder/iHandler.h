#ifndef I_DECODER_H
#define I_DECODER_H

#include "com\message_format.h"

namespace communication {

class iHandler {
public:
  iHandler() = default;
  virtual ~iHandler() = default;

  virtual bool run(SERIAL_MSG_TYPE type) = 0;

};


}

#endif