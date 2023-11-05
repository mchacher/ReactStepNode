#include <Arduino.h>
#include "rf.h"
#include "../app_config.h"
#if REACT_MESH == 1
#include "ArduinoLog.h"
#include "decoder/eventHandler.h"
#include "decoder/reactCodeHandler.h"
#include "react/event_registry.h"
#include "react/event_type.h"
#include "../state_machine.h"


namespace communication
{
  const uint8_t RFNode::DEFAULT_NODE_ID = 2;
  bool RFNode::_isReady = false;

  RFNode::RFNode() : _radio(PIN_CE, PIN_CSN),
             _network(_radio),
             _mesh(_radio, _network),
             _nodeID(DEFAULT_NODE_ID)
  {
    _handlerList.push_back(std::make_shared<eventHandler>(_network));
    _handlerList.push_back(std::make_shared<reactCodeHandler>(_network));
  } 

  void RFNode::setup()
  {
    Log.verboseln(F("[%s] Node ID [%d]"), __func__, _mesh.getNodeID());
    SPI.setRX(PIN_MISO);
    SPI.setTX(PIN_MOSI);
    SPI.setSCK(PIN_SCK);
    SPI.begin();
    if (!_radio.begin(&SPI))
    {
      Log.noticeln(F("[%s] radio hardware is not responding!!"), __func__);
      while (1)
        ;
    }
    _radio.setPALevel(RF24_PA_MIN, 0);
    _mesh.setNodeID(_nodeID);
    if (!_mesh.begin())
    {
      if (_radio.isChipConnected())
      {
        do
        {
          // mesh.renewAddress() will return MESH_DEFAULT_ADDRESS on failure to connect
          Log.noticeln(F("Could not connect to network.\nConnecting to the mesh..."));
        } while (_mesh.renewAddress() == MESH_DEFAULT_ADDRESS);
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
    _isReady = true;
    Log.noticeln(F("[%s] RF communication ready with Node ID: [%i]"), __func__, _mesh.getNodeID());
  }

  void RFNode::receive()
  {
    // _mesh.update();
    // Check for incoming data from the sensors
    if (_network.available())
    {
      RF24NetworkHeader header;
      RF_PACKET packet;
      uint8_t size = _network.read(header, (uint8_t *)&packet);
      Log.noticeln(F("[%s]: message from Node ID [%d], type [%i]"), __func__, header.from_node, header.type);

      bool isManaged(false);

      for (const auto &handler : _handlerList)
      {
        isManaged = handler->run(static_cast<RFMessageType>(header.type), (uint8_t *)&packet, size);
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
        // _network.read(header, 0, 0); .... ????
        Log.noticeln(F("[%s] Received unmanaged data type [%d]"), __func__, header.type);
      }
    }
  }

  bool RFNode::send(uint16_t destNode, uint8_t type, uint8_t *packet, uint8_t size)
  {
    bool isSuccess(false);
    if (_isReady)
    {
      // Call mesh.update to keep the network updated
      _mesh.update();

      if (!_mesh.write(destNode, packet, type, size))
      {
        // If a write fails, check connectivity to the mesh network
        if (!_mesh.checkConnection())
        {
          // The address could be refreshed per a specified timeframe or only when sequential writes fail, etc.
          do
          {
            Log.noticeln(F("[%s] Renewing Address..."), __func__);
          } while (_mesh.renewAddress() == MESH_DEFAULT_ADDRESS);
        }
        else
        {
          Log.noticeln(F("[%s] Failed to to send to node [%d]"), __func__, _nodeID);
        }
      }
      else
      {
        Log.verboseln(F("[%s] Send OK - to node [%d]"), __func__, _nodeID);
        isSuccess = true;
      }
    }
    else
    {
      Log.noticeln(F("[%s] Communication stask isn't ready to send data"), __func__);
    }
    return isSuccess;
  }

  uint16_t RFNode::getNodeId()
  {
    return _nodeID;
  }

  void RFNode::incrementNodeId()
  {
    _nodeID = (_nodeID + 1) % (MAX_NODES - 1);

    // Node ID 0 and 1 are reserved for Master
    // ???  @Adrien: pourquoi 1 est réservé pour le Master ???
    // dans le doute j'ai commenté ton code ci-dessous
    if (0 == _nodeID)
    {
      _nodeID = 1;
    }
    // if (1 == _nodeID)
    // {
    //   _nodeID = 2;
    // }
  }

  void RFNode::setNodeID(uint8_t id)
  {
    _nodeID = id;
    // _mesh.setNodeID(_nodeID);
  }

  void RFNode::releaseAddress()
  {
    _mesh.releaseAddress();
  }

  // void RFNode::buildHeader(PACKET_HEADER& header, MSG_TYPE type, uint8_t data_length)
  // {
  //   header.packet_id = generatePacketId();
  //   header.type = type;
  //   header.data_length = data_length;
  // }

  // uint16_t RFNode::generatePacketId()
  // {
  //   if (0xFFFF <= _currentPacketId + 1)
  //   {
  //     _currentPacketId = 0;
  //   }
  //   return _currentPacketId++;
  // }

  void RFNode::task()
  {
    static uint16_t tick = 0;
    tick += 1;
    // Call mesh.update to keep the network updated
    _mesh.update();
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
