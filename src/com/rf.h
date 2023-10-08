#include "hardware_config.h"
#if REACT_MESH == 1

#ifndef RF_H
#define RF_H

#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>

#define MAX_NODE_ID 99
#define MESH_NOMASTER
namespace communication {
struct payload_t
{
  unsigned long ms;
  unsigned long counter;
};

typedef enum
{
  SERIAL_MSG_TYPE_NULL = 0,
  SERIAL_MSG_TYPE_LOG = 1,
  SERIAL_MSG_TYPE_SYS = 2,
  SERIAL_MSG_TYPE_FILE = 3, // for file transfer from a computer
  SERIAL_MSG_TYPE_EVENT = 4
} MSG_TYPE;


class rf {
public:
  rf();
  virtual ~rf() = default;

  /*!********************************************************************
   * @brief Initialized all requested object for the communication
   **********************************************************************/
  void setup();

  /*!********************************************************************
   * @brief Receive data from RF communication
   **********************************************************************/
  void receive();

  /*!********************************************************************
   * @brief Send data over RF24
   * @param destNode the address of destination data
   * @param data payload to be send
   * @return bool in success even false
   **********************************************************************/
  bool send(uint16_t destNode, uint32_t data);

  /*!********************************************************************
   * @brief Return the current Node Id
   * @return uint16_t NodeId
   **********************************************************************/
  uint16_t getNodeId();

  /*!********************************************************************
   * @brief Increment the Node ID
   **********************************************************************/
  void incrementNodeId();

private:
  RF24 mRadio;
  RF24Network mNetwork;
  RF24Mesh mMesh;

#if RP2040 1
  arduino::MbedSPI SpiRP2040(PIN_MISO, PIN_MOSI, PIN_SCK);
#endif

  uint8_t mNodeId;

  static const uint8_t DEFAULT_NODE_ID;
};
}

#endif
#endif