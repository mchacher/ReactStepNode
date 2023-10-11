#ifndef I_DECODER_H
#define I_DECODER_H

namespace communication {

class IDecoder {
public:
  IDecoder() = default;
  virtual ~IDecoder() = default;

  virtual void run(SERIAL_MSG_TYPE type, const void* data) = 0;

};


}

#endif