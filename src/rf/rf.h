#include "hardware_config.h"
#include "app_config.h"

#if REACT_MESH == 1

#ifndef RF_H
#define RF_H

#include "decoder/iHandler.h"
#include "react_air_protocol.h"
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
  class RFNode
  {
  public:
    /**
     * @brief Constructor for the RF class.
     */
    RFNode();

    /**
     * @brief Destructor for the RF class.
     */
    virtual ~RFNode() = default;

    /**
     * @brief Initialize all required objects for communication.
     */
    void setup();

    /**
     * @brief Task for RF communication.
     */
    void task();

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

    void releaseAddress();

  private:
    RF24 _radio;
    RF24Network _network;
    RF24Mesh _mesh;


    uint8_t _nodeID;
    static const uint8_t DEFAULT_NODE_ID;
    static bool _isReady;
    std::vector<std::shared_ptr<iHandler>> _handlerList;
  };
}

#endif

static communication::RFNode comm;

#endif
