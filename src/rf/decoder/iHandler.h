#ifndef I_DECODER_H
#define I_DECODER_H

#include "rf/react_air_protocol.h"
namespace communication
{

  class iHandler
  {
  public:
    iHandler() = default;
    virtual ~iHandler() = default;

    virtual bool run(RFMessageType type, uint8_t *packet, uint8_t size) = 0;
  };

}

#endif