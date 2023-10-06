#include "hardware_config.h"
#if REACT_MESH == 1

#ifndef RF_H
#define RF_H

#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>

#define MAX_NODE_ID 99
#define MESH_NOMASTER
struct payload_t
{
  unsigned long ms;
  unsigned long counter;
};
namespace communication {

class rf {
public:
  rf();
  virtual ~rf() = default;

  /*!********************************************************************
   * @brief Initialized all requested object for the communication
   **********************************************************************/
  void setup();

  void receive();

  bool send(uint16_t destNode, uint32_t data);

  uint16_t getNodeId();

  void incrementNode_id();

private:
  RF24 mRadio;
  RF24Network mNetwork;
  RF24Mesh mMesh;

  uint8_t mNodeId;

  static const uint8_t DEFAULT_NODE_ID;
};
}

#endif
#endif