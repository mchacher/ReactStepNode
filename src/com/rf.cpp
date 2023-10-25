#include <Arduino.h>
#include "rf.h"
#include "../app_config.h"
#if REACT_MESH == 1
#include "ArduinoLog.h"
#include "decoder/eventHandler.h"
#include "decoder/reactCodeHandler.h"
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
             mNodeId(DEFAULT_NODE_ID),
             mCurrentPacketId(0)
  {
    mHandlerList.push_back(std::make_shared<eventHandler>(mNetwork));
    mHandlerList.push_back(std::make_shared<reactCodeHandler>(mNetwork));
  } 

  void rf::setup()
  {
    Log.verboseln(F("[%s] Node ID [%d]"), __func__, mMesh.getNodeID());
    SPI.setRX(PIN_MISO);
    SPI.setTX(PIN_MOSI);
    SPI.setSCK(PIN_SCK);
    SPI.begin();
    if (!mRadio.begin(&SPI))
    {
      Log.noticeln(F("[%s] radio hardware is not responding!!"), __func__);
      while (1)
        ;
    }
    mRadio.setPALevel(RF24_PA_MIN, 0);
    mMesh.setNodeID(mNodeId);
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
    Log.noticeln(F("[%s] RF communication ready with Node ID: [%i]"), __func__, mMesh.getNodeID());
  }

  void rf::receive()
  {
    // mMesh.update();
    // Check for incoming data from the sensors
    if (mNetwork.available())
    {
      RF24NetworkHeader header;
      RF_PACKET packet;
      uint8_t size = mNetwork.read(header, (uint8_t *)&packet);
      Log.noticeln(F("[%s]: message from Node ID [%d], type [%i]"), __func__, header.from_node, header.type);

      bool isManaged(false);

      for (const auto &handler : mHandlerList)
      {
        isManaged = handler->run(static_cast<MSG_TYPE>(header.type), (uint8_t *)&packet, size);
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
        // mNetwork.read(header, 0, 0); .... ????
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
    mNodeId = (mNodeId + 1) % (MAX_NODES - 1);

    // Node ID 0 and 1 are reserved for Master
    // ???  @Adrien: pourquoi 1 est réservé pour le Master ???
    // dans le doute j'ai commenté ton code ci-dessous
    if (0 == mNodeId)
    {
      mNodeId = 1;
    }
    // if (1 == mNodeId)
    // {
    //   mNodeId = 2;
    // }
  }

  void rf::setNodeID(uint8_t id)
  {
    mNodeId = id;
    // mMesh.setNodeID(mNodeId);
  }

  void rf::releaseAddress()
  {
    mMesh.releaseAddress();
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

  void rf::rf_task()
  {
    static uint16_t tick = 0;
    tick += 1;
    // Call mesh.update to keep the network updated
    mMesh.update();
    if ((tick % PERIOD_HEARTBEAT) == 0)
    {
      Log.verboseln(F("[%s]: updating mesh network and sending heartbeat"), __func__);
      // Send heartbeat
      RF_PACKET packet;
      packet.payload[0] = state_machine_get_active_state();
      if (this->send(0, MSG_TYPE_HEARTBEAT, (uint8_t *)&packet, 1))
      {
        Log.verboseln(F("[%s]: successfully sent heartbeat"), __func__);
      }
    }
    // check if any message received
    this->receive();
  }
}
#endif
