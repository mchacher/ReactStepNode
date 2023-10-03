#include "hardware_config.h"
#include "ArduinoLog.h"
#include <Arduino.h>
#include "rf.h"

namespace communication
{
  const rf24_gpio_pin_t rf::PIN_CE = 7;
  const rf24_gpio_pin_t rf::PIN_CSN = 8;
  const uint8_t rf::DEFAULT_NODE_ID = 2;

#if REACT_MESH == 1

  rf::rf() : mRadio(PIN_CE, PIN_CSN),
             mNetwork(mRadio),
             mMesh(mRadio, mNetwork),
             mNodeIdIsSet(false)
  {
    // Constructor for RF class when REACT_MESH is defined
  }

  void rf::setup()
  {
    if (!mNodeIdIsSet)
    {
      // Set the node ID if it hasn't been set before
      mMesh.setNodeID(DEFAULT_NODE_ID);
      mNodeIdIsSet = true;
    }

    Log.noticeln(F("[%s] Node ID [%d]"), __func__, mMesh.getNodeID());

    // Connect to the mesh
    if (!mMesh.begin())
    {
      if (mMesh.getNodeID() != 0 && mRadio.isChipConnected())
      {
        do
        {
          // mesh.renewAddress() will return MESH_DEFAULT_ADDRESS on failure to connect
          Log.noticeln(F("Could not connect to network. Connecting to the mesh..."));
        } while (mMesh.renewAddress() == MESH_DEFAULT_ADDRESS);
      }
      else
      {
        Log.noticeln(F("Radio hardware not responding."));
        while (1)
        {
          // Hold in an infinite loop or manage this case using screen to show a user message.
        }
      }
    }
    Log.noticeln(F("[%s] Communication stack READY"), __func__);
  }

  void rf::receive()
  {
    // Call mesh.update to keep the network updated
    mMesh.update();
    if (mMesh.getNodeID() == 0)
    {
      mMesh.DHCP();
    }

    // Check for incoming data from the sensors
    if (mNetwork.available())
    {
      RF24NetworkHeader header;
      mNetwork.peek(header);
      Log.noticeln(F("[%s] Node ID [%d]"), __func__, header.from_node - 1);
      uint32_t data(0);
      switch (header.type)
      {
      case 'M':
        mNetwork.read(header, &data, sizeof(data));
        Log.noticeln(F("Data = [%lu]"), data);
        break;
      default:
        mNetwork.read(header, 0, 0);
        Log.noticeln(F("[%s] Received unmanaged data type [%d]"), __func__, header.type);
        break;
      }
    }
  }

  bool rf::send(uint16_t destNode, uint32_t data)
  {
    bool isSuccess(false);

    // Call mesh.update to keep the network updated
    mMesh.update();

    if (!mMesh.write(&data, 'M', sizeof(data)))
    {
      // If a write fails, check connectivity to the mesh network
      if (!mMesh.checkConnection())
      {
        // The address could be refreshed per a specified timeframe or only when sequential writes fail, etc.
        do
        {
          Log.noticeln(F("[%s] Renewing Address..."), __func__);
        } while (mMesh.renewAddress() == MESH_DEFAULT_ADDRESS);
      }
      else
      {
        Log.noticeln(F("[%s] Send fail, Test OK"), __func__);
      }
    }
    else
    {
      Log.noticeln(F("[%s] Send OK - [%d] Data = [%lu]"), __func__, mMesh.getNodeID(), data);
      isSuccess = true;
    }

    return isSuccess;
  }

  uint16_t rf::getNodeId()
  {
    return mMesh.getNodeID();
  }

  void rf::incrementNode_id()
  {
    mNodeIdIsSet = true;
    uint16_t nodeId = mMesh.getNodeID();
    nodeId = (nodeId % MAX_NODE_ID) + 1;

    // Node ID 0 and 1 are reserved for Master
    if (0 == nodeId)
    {
      nodeId = 2;
    }
    if (1 == nodeId)
    {
      nodeId = 2;
    }

    mMesh.setNodeID(nodeId);
  }

#else

  uint8_t nodeId; // Define the static member variable

  rf::rf()
  {
    nodeId = 1; // Initialize the static member variable
  }

  void rf::setup()
  {
    Log.noticeln(F("[%s] Communication stack READY"), __func__);
  }

  void rf::receive()
  {
    // Implement receive logic here
  }

  bool rf::send(uint16_t destNode, uint32_t data)
  {
    // Implement send logic here
    return true;
  }

  uint16_t rf::getNodeId()
  {
    return nodeId;
  }

  void rf::incrementNode_id()
  {
    nodeId++;
  }

#endif
}
