#include <Arduino.h>
#include "rf.h"
#include "../app_config.h"
#if REACT_MESH == 1
#include "ArduinoLog.h"
#include "decoder/eventHandler.h"
#include "../reactmagic/event_registry.h"
#include "../reactmagic/event_type.h"
#include "../state_machine.h"
namespace communication
{
  const uint8_t rf::DEFAULT_NODE_ID = 2;
  bool rf::mIsReady = false;

  rf::rf() : mRadio(PIN_CE, PIN_CSN),
             mNetwork(mRadio),
             mMesh(mRadio, mNetwork),
#if RP2040 == 1
             SPI0(PIN_MISO, PIN_MOSI, PIN_SCK),
#endif
             mNodeId(DEFAULT_NODE_ID),
             mCurrentPacketId(0)
  {
    mDecoderList.push_back(std::make_shared<eventHandler>(mNetwork));
  }

  void rf::setup()
  {
    Log.noticeln(F("[%s] Node ID [%d]"), __func__, mNodeId);
#if RP2040 == 1
    SPI0.begin();
    if (!mRadio.begin(&SPI0))
    {
      Log.noticeln(F("[%s] radio hardware is not responding!!"), __func__);
      while (1)
        ;
    }
#endif
    mRadio.setPALevel(RF24_PA_MIN, 0);

    if (!mMesh.begin())
    {
      if (mRadio.isChipConnected())
      {
        do
        {
          // mesh.renewAddress() will return MESH_DEFAULT_ADDRESS on failure to connect
          Log.noticeln(F("Could not connect to network.\nConnecting to the mesh..."));
        } while (mMesh.renewAddress() == MESH_DEFAULT_ADDRESS);
      }
      else
      {
        Log.noticeln(F("Radio hardware not responding."));
        while (1)
        {
          // hold in an infinite loop
        }
      }
    }
    mIsReady = true;
    Log.noticeln(F("[%s] Communication stack READY"), __func__);
  }

  void rf::receive()
  {
    // mMesh.update();
    // Check for incoming data from the sensors
    if (mNetwork.available())
    {
      RF24NetworkHeader header;
      RF_PACKET packet;
      uint8_t size = mNetwork.read(header, (uint8_t*)&packet);
      Log.noticeln(F("[%s]: message from Node ID [%d]"), __func__, header.from_node);

      bool isManaged(false);

      for (const auto &decoder : mDecoderList)
      {
        isManaged = decoder->run(static_cast<MSG_TYPE>(header.type), (uint8_t*)&packet, size);
        if (isManaged)
        {
          // The first object which can handle this message will finish this transaction
          // because in run method we clear receive buffer
          break;
        }
      }
      // In case of unmanageable data, just clear the receive buffer
      if (!isManaged)
      {
        mNetwork.read(header, 0, 0);
        Log.noticeln(F("[%s] Received unmanaged data type [%d]"), __func__, header.type);
      }
    }
  }

  bool rf::send(uint16_t destNode, uint8_t type, uint8_t *packet, uint8_t size)
  {
    bool isSuccess(false);
    if (mIsReady)
    {
      // Call mesh.update to keep the network updated
      mMesh.update();

      if (!mMesh.write(destNode, packet, type, size))
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
          Log.noticeln(F("[%s] Failed to to send to node [%d]"), __func__, mNodeId);
        }
      }
      else
      {
        Log.verboseln(F("[%s] Send OK - to node [%d]"), __func__, mNodeId);
        isSuccess = true;
      }
    }
    else
    {
      Log.noticeln(F("[%s] Communication stask isn't ready to send data"), __func__);
    }
    return isSuccess;
  }

  uint16_t rf::getNodeId()
  {
    return mNodeId;
  }

  void rf::incrementNodeId()
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

  // void rf::buildHeader(PACKET_HEADER& header, MSG_TYPE type, uint8_t data_length)
  // {
  //   header.packet_id = generatePacketId();
  //   header.type = type;
  //   header.data_length = data_length;
  // }

  // uint16_t rf::generatePacketId()
  // {
  //   if (0xFFFF <= mCurrentPacketId + 1)
  //   {
  //     mCurrentPacketId = 0;
  //   }
  //   return mCurrentPacketId++;
  // }

#define PERIOD_HEARTBEAT (2000 / TASK_CYCLE_FAST) // every 2s, assuming 20ms as fast cycle (TODO, check what is the right timing)

  void rf::rf_task()
  {
    static uint16_t tick = 0;
    tick += 1;
    // Call mesh.update to keep the network updated
    mMesh.update();
    //  Log.noticeln(F("[%s]: tick %i / %i"), __func__, tick, PERIOD_MESH_UPDATE);
    if ((tick % PERIOD_HEARTBEAT) == 0)
    {
      Log.verboseln(F("[%s]: updating mesh network and sending heartbeat"), __func__);
      // Send heartbeat
      RF_PACKET packet;
      packet.payload[0] = state_machine_get_active_state();
      if (this->send(0, MSG_TYPE_HEARTBEAT, (uint8_t *)&packet, 1))
      {
        Log.noticeln(F("[%s]: successfully sent heartbeat"), __func__);
      }
      
    }
  }
}
#endif
