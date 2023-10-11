#include "hardware_config.h"
#if REACT_MESH == 1

#ifndef RF_H
#define RF_H

#include "com/message_format.h"
#include <RF24.h>
#include <RF24Mesh.h>
#include <RF24Network.h>

#define MAX_NODE_ID 99
#define MESH_NOMASTER
namespace communication {
struct payload_t
{
  unsigned long ms;
  unsigned long counter;
};


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
  bool send(uint16_t destNode, const void* data, SERIAL_MSG_TYPE type);

  /*!********************************************************************
   * @brief Send data over RF24 to master
   * @param data payload to be send
   * @param type data type
   * @return bool in success even false
   **********************************************************************/
  bool masterSend(const void* data, SERIAL_MSG_TYPE type);

  /*!********************************************************************
   * @brief Return the current Node Id
   * @return uint16_t NodeId
   **********************************************************************/
  uint16_t getNodeId();

  /*!********************************************************************
   * @brief Increment the Node ID
   **********************************************************************/
  void incrementNodeId();

  /*!********************************************************************
   * @brief Useful to build communication header
   * @param header
   **********************************************************************/
  void buildHeader(PACKET_HEADER& header, SERIAL_MSG_TYPE type, uint8_t data_length);

protected:

  void eventMgt(const PACKET_EVENT& data);

  uint16_t generatePacketId();

private:
  RF24 mRadio;
  RF24Network mNetwork;
  RF24Mesh mMesh;

#if RP2040 == 1
  arduino::MbedSPI mSpiRP2040;
#endif

  uint8_t mNodeId;

  static const uint8_t DEFAULT_NODE_ID;

  static bool mIsReady;

  uint16_t mCurrentPacketId;
};


}

#endif
#endif
static communication::rf comm;