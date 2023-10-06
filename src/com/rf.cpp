#include "hardware_config.h"
#if REACT_MESH == 1
#include "ArduinoLog.h"
#include <Arduino.h>
#include "rf.h"

namespace communication
{
  const uint8_t rf::DEFAULT_NODE_ID = 2;
  arduino::MbedSPI SPI0(PIN_MISO, PIN_MOSI, PIN_SCK);

  rf::rf() : mRadio(PIN_CE, PIN_CSN),
             mNetwork(mRadio),
             mMesh(mRadio, mNetwork),
             mNodeId(DEFAULT_NODE_ID)
  {
    // Constructor for RF class when REACT_MESH is defined
  }

  void rf::setup()
  {
    Log.noticeln(F("[%s] Node ID [%d]"), __func__, mNodeId);
    SPI0.begin();
    if (!mRadio.begin(&SPI0))
    {
      Log.noticeln(F("radio hardware is not responding!!"));
      while (1)
        ;
    }
    // Connect to the mesh
    if (!mMesh.begin())
    {
      if (0 != mNodeId && mRadio.isChipConnected())
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
    // todo: move it in dedicated method to be call each about 2 seconds
    mMesh.update();

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
      Log.noticeln(F("[%s] Send OK - [%d] Data = [%lu]"), __func__, mNodeId, data);
      isSuccess = true;
    }
    return isSuccess;
  }

  uint16_t rf::getNodeId()
  {
    return mNodeId;
  }

  void rf::incrementNode_id()
  {
    mNodeId = (mNodeId + 1) % MAX_NODE_ID;

    // Node ID 0 and 1 are reserved for Master
    if (0 == mNodeId)
    {
      mNodeId = 2;
    }
    if (1 == mNodeId)
    {
      mNodeId = 2;
    }

    mMesh.setNodeID(mNodeId);
  }

}
#endif
