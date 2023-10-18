#include "hardware_config.h"
#include "app_config.h"
#if REACT_MESH == 1

#ifndef RF_H
#define RF_H

#include "decoder/iHandler.h"
#include "message_format.h"
#include <RF24.h>
#include <RF24Mesh.h>
#include <RF24Network.h>
#include <vector>

#define MAX_NODE_ID 99
#define MESH_NOMASTER
namespace communication
{
  struct payload_t
  {
    unsigned long ms;
    unsigned long counter;
  };

  class rf
  {
  public:
    rf();
    virtual ~rf() = default;

    /*!********************************************************************
     * @brief Initialized all requested object for the communication
     **********************************************************************/
    void setup();

    void rf_task();

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
    bool send(uint16_t destNode, uint8_t type, uint8_t *packet, uint8_t size);

    /*!********************************************************************
     * @brief Return the current Node Id
     * @return uint16_t NodeId
     **********************************************************************/
    uint16_t getNodeId();

    /*!********************************************************************
     * @brief Increment the Node ID
     **********************************************************************/
    void incrementNodeId();

    void setNodeID(uint8_t id);

    // /*!********************************************************************
    //  * @brief Useful to build communication header
    //  * @param header
    //  **********************************************************************/
    // void buildHeader(PACKET_HEADER& header, MSG_TYPE type, uint8_t data_length);

    // protected:

    //   uint16_t generatePacketId();

  private:
    RF24 mRadio;
    RF24Network mNetwork;
    RF24Mesh mMesh;

#if RP2040 == 1
    arduino::MbedSPI SPI0;
#endif

    uint8_t mNodeId;

    static const uint8_t DEFAULT_NODE_ID;

    static bool mIsReady;

    uint16_t mCurrentPacketId;

    std::vector<std::shared_ptr<iHandler>> mDecoderList;
  };

}

#endif
static communication::rf comm;

#endif
