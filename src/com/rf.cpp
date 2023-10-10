#include "hardware_config.h"
#if REACT_MESH == 1
#include "ArduinoLog.h"
#include "reactmagic\event_registry.h"
#include "reactmagic\event_type.h"
#include "rf.h"
#include <Arduino.h>

namespace communication
{
const uint8_t rf::DEFAULT_NODE_ID = 2;
bool rf::mIsReady = false;

rf::rf() :
  mRadio(PIN_CE, PIN_CSN),
  mNetwork(mRadio),
  mMesh(mRadio, mNetwork),
  mNodeId(DEFAULT_NODE_ID)
{
  // Constructor for RF class when REACT_MESH is defined
}

void rf::setup()
{
  Log.noticeln(F("[%s] Node ID [%d]"), __func__, mNodeId);
#if RP2040 == 1
  SpiRP2040.begin();
  if (!mRadio.begin(&SpiRP2040))
  {
    Log.noticeln(F("[%s] radio hardware is not responding!!"), __func__);
    while (1)
      ;
  }
#endif
  // Connect to the mesh
  if (!mMesh.begin())
  {
    if (0 != mNodeId && mRadio.isChipConnected())
    {
      do
      {
        // mesh.renewAddress() will return MESH_DEFAULT_ADDRESS on failure to connect
        Log.noticeln(F("[%s] Could not connect to network. Connecting to the mesh..."), __func__);
      } while (mMesh.renewAddress() == MESH_DEFAULT_ADDRESS);
    }
    else
    {
      Log.noticeln(F("[%s] Radio hardware not responding."), __func__);
      while (1)
      {
        // Hold in an infinite loop or manage this case using screen to show a user message.
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
    mNetwork.peek(header);
    Log.noticeln(F("[%s] Node ID [%d]"), __func__, header.from_node);

    switch (static_cast<SERIAL_MSG_TYPE>(header.type)) {
    case SERIAL_MSG_TYPE_HEARTBEAT: {
      PACKET_HEADER data;
      mNetwork.read(header, &data);
#ifdef DEBUG
      log_info(F("[%s] Heart beat message"), __func__);
#endif
      break;
    }
    case SERIAL_MSG_TYPE_LOG:
      // #ifdef DEBUG
      //       log_info(F("[%s] Log message"), __func__);
      // #endif
      break;
    case SERIAL_MSG_TYPE_SYS:
      // #ifdef DEBUG
      //       log_info(F("[%s] System message"), __func__);
      // #endif
      break;
    case SERIAL_MSG_TYPE_FILE:
      // #ifdef DEBUG
      //       log_info(F("[%s] File message"), __func__);
      // #endif
      break;
    case SERIAL_MSG_TYPE_EVENT: {
      PACKET_EVENT data;
      mNetwork.read(header, &data);
      eventMgt(data);
#ifdef DEBUG
      log_info(F("[%s] Event message"), __func__);
#endif
      break;
    }
    default:
      mNetwork.read(header, 0, 0);
      Log.noticeln(F("[%s] Received unmanaged data type [%d]"), __func__, header.type);
      break;
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

bool rf::masterSend(const void* data, SERIAL_MSG_TYPE type) {
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

void rf::eventMgt(const PACKET_EVENT& data) {
  Log.noticeln(F("[%s] Receive event [%d]"), __func__, data.event);
  switch (static_cast<EVENT_TYPE>(data.event))
  {
  case EVENT_SYS_TYPE_START:
    event_registry_push(EVENT_SYS_TYPE_START);
    Log.noticeln(F("[%s] Send internal Start event"), __func__, data.event);
    break;

  default:
    Log.noticeln(F("[%s] Not managed event"), __func__, data.event);
    break;
  }
}

}
#endif
