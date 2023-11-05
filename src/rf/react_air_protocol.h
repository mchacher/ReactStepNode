#ifndef REACT_AIR_PROTOCOL
#define REACT_AIR_PROTOCOL 
 
 #include "Arduino.h"
 
 /**
  * @brief RF msg types - shall be between 0 to 127 as per RF24MEsh requirements
  * Message types 1 - 64 (decimal) will NOT be acknowledged by the network, types 65 - 127 will be. 
  * Use as appropriate 
  *
  */
enum class RFMessageType
{
  REACT_EVENT = 4,
  REACT_CODE = 65
};


typedef enum
{
  MSG_TYPE_HEARTBEAT = 0,
  MSG_TYPE_LOG = 1,
  MSG_TYPE_COMMAND = 2,
  MSG_TYPE_FILE = 3, // for file transfer from a computer
  MSG_TYPE_EVENT = 4,
  MSG_TYPE_ACK = 5,
  MSG_TYPE_REACT_CODE = 6
} MSG_TYPE;

typedef struct __attribute__((__packed__))
{
  uint8_t payload[256];
} RF_PACKET;


#endif