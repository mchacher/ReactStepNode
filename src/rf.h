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
#if REACT_MESH == 1
  RF24 mRadio;
  RF24Network mNetwork;
  RF24Mesh mMesh;
#endif

  bool mNodeIdIsSet;

  static const rf24_gpio_pin_t PIN_CE;
  static const rf24_gpio_pin_t PIN_CSN;
  static const uint8_t DEFAULT_NODE_ID;
};
}

#endif