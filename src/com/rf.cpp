#include <Arduino.h>
#include "rf.h"
#if REACT_MESH == 1
#include "ArduinoLog.h"
#include "decoder/eventHandler.h"
#include "../reactmagic/event_registry.h"
#include "../reactmagic/event_type.h"

namespace communication
{
const uint8_t rf::DEFAULT_NODE_ID = 2;
bool rf::mIsReady = false;

rf::rf() :
  mRadio(PIN_CE, PIN_CSN),
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
        Serial.println(F("Could not connect to network.\nConnecting to the mesh..."));
      } while (mMesh.renewAddress() == MESH_DEFAULT_ADDRESS);
    }
    else
    {
      Serial.println(F("Radio hardware not responding."));
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
  // Call mesh.update to keep the network updated
  // todo: move it in dedicated method to be call each about 2 seconds
  mMesh.update();

  // Check for incoming data from the sensors
  if (mNetwork.available())
  {
    RF24NetworkHeader header;
    uint8_t packet[256];
    uint8_t size = mNetwork.read(header, &packet);
    Log.noticeln(F("[%s]: message from Node ID [%d]"), __func__, header.from_node);

    bool isManaged(false);

    for (const auto& decoder : mDecoderList)
    {
      isManaged = decoder->run(static_cast<SERIAL_MSG_TYPE>(header.type), packet, size);
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

bool rf::send(uint16_t destNode, const void* data, SERIAL_MSG_TYPE type)
{
  bool isSuccess(false);
  if (mIsReady) {
    // Call mesh.update to keep the network updated
    mMesh.update();

    if (!mMesh.write(&data, type, sizeof(data)))
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
  }
  else
  {
    Log.noticeln(F("[%s] Communication stask isn't ready to send data"), __func__);
  }
  return isSuccess;
}

bool rf::masterSend(const void* data, SERIAL_MSG_TYPE type)
{
  return send(0, data, type);
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

void rf::buildHeader(PACKET_HEADER& header, SERIAL_MSG_TYPE type, uint8_t data_length)
{
  header.packet_id = generatePacketId();
  header.type = type;
  header.data_length = data_length;
}

uint16_t rf::generatePacketId()
{
  if (0xFFFF <= mCurrentPacketId + 1)
  {
    mCurrentPacketId = 0;
  }
  return mCurrentPacketId++;
}

}
#endif
