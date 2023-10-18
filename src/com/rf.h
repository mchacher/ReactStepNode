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
#include <memory> 

#define MESH_NOMASTER

namespace communication
{

  /**
   * @brief Structure for payload data.
   */
  struct payload_t
  {
    unsigned long ms;      /**< Timestamp */
    unsigned long counter; /**< Counter */
  };

  /**
   * @brief RF communication class.
   */
  class rf
  {
  public:
    /**
     * @brief Constructor for the RF class.
     */
    rf();

    /**
     * @brief Destructor for the RF class.
     */
    virtual ~rf() = default;

    /**
     * @brief Initialize all required objects for communication.
     */
    void setup();

    /**
     * @brief Task for RF communication.
     */
    void rf_task();

    /**
     * @brief Receive data from RF communication.
     */
    void receive();

    /**
     * @brief Send data over RF24.
     * @param destNode Address of the destination node.
     * @param type Type of the data.
     * @param packet Pointer to the data payload to send.
     * @param size Size of the data payload.
     * @return True if sending was successful, false otherwise.
     */
    bool send(uint16_t destNode, uint8_t type, uint8_t *packet, uint8_t size);

    /**
     * @brief Get the current Node ID.
     * @return Current Node ID.
     */
    uint16_t getNodeId();

    /**
     * @brief Increment the Node ID.
     */
    void incrementNodeId();

    /**
     * @brief Set the Node ID.
     * @param id New Node ID to set.
     */
    void setNodeID(uint8_t id);

  private:
    RF24 mRadio;
    RF24Network mNetwork;
    RF24Mesh mMesh;


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
