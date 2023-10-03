#include "hardware_config.h"
#if REACT_MESH == 1
#include "ArduinoLog.h"
#include <Arduino.h>
#include "rf.h"

#define DEBUG

namespace communication {

const rf24_gpio_pin_t rf::PIN_CE = 7;
const rf24_gpio_pin_t rf::PIN_CSN = 8;
const uint8_t rf::DEFAULT_NODE_ID = 2;

rf::rf() :
  mRadio(PIN_CE, PIN_CSN),
  mNetwork(mRadio),
  mMesh(mRadio, mNetwork),
  mNodeIdIsSet(false)
{
}

void rf::setup() {
  Serial.begin(115200);
  while (!Serial) {
    // some boards need this because of native USB capability
  }

  if (false == mNodeIdIsSet) {
    mMesh.setNodeID(DEFAULT_NODE_ID);
  }

#ifdef DEBUG
  Log.noticeln(F("[%s] Node ID [%d]"), __func__, mMesh.getNodeID());
#endif

  // Connect to the mesh
  if (!mMesh.begin()) {
    if (0 != mMesh.getNodeID() && mRadio.isChipConnected()) {
      do {
        // mesh.renewAddress() will return MESH_DEFAULT_ADDRESS on failure to connect
        Log.noticeln(F("Could not connect to network.Connecting to the mesh..."));
      } while (mMesh.renewAddress() == MESH_DEFAULT_ADDRESS);
    }
    else {
      Log.noticeln(F("Radio hardware not responding."));
      while (1) {
        // hold in an infinite loop
        // TODO: manage this case using screen to show user message
      }
    }

  }
#ifdef DEBUG
  Log.noticeln(F("[%s] Communication stack READY"), __func__);
#endif
}

void rf::receive() {

  // Call mesh.update to keep the network updated
  mMesh.update();
  if (0 == mMesh.getNodeID()) {
    mMesh.DHCP();
  }

  // Check for incoming data from the sensors
  if (mNetwork.available()) {
    RF24NetworkHeader header;
    mNetwork.peek(header);
#ifdef DEBUG
    Log.noticeln(F("[%s] Node ID [%d] "), __func__, header.from_node - 1);
#endif

    uint32_t data(0);
    switch (header.type) {
    case 'M':
      mNetwork.read(header, &data, sizeof(data));
#ifdef DEBUG
      Log.noticeln(F("Data = [%lu]"), data);
#endif
      break;
    default:
      mNetwork.read(header, 0, 0);
#ifdef DEBUG
      Log.noticeln(F("[%s] Received unmanaged data type [%d]"), __func__, header.type);
#endif
      break;
    }
  }
}

bool rf::send(uint16_t destNode, uint32_t data) {
  bool isSuccess(false);

  // Call mesh.update to keep the network updated
  mMesh.update();

  if (!mMesh.write(&data, 'M', sizeof(data))) {
    // If a write fails, check connectivity to the mesh network
    if (!mMesh.checkConnection()) {
      // The address could be refreshed per a specified timeframe or only when sequential writes fail, etc.
      do {
        Log.noticeln(F("[%s] Renewing Address..."), __func__);
      } while (mMesh.renewAddress() == MESH_DEFAULT_ADDRESS);
    }
#ifdef DEBUG
    else {
      Log.noticeln(F("[%s] Send fail, Test OK"), __func__);
    }
#endif
  }
  else {
#ifdef DEBUG
    Log.noticeln(F("[%s] Send OK - [%d] Data = [%lu]"), __func__, mMesh.getNodeID(), data);
#endif
    isSuccess = true;
  }

  return isSuccess;
}

uint16_t rf::getNodeId() {
  return mMesh.getNodeID();
}

void rf::incrementNode_id() {
  mNodeIdIsSet = true;
  uint16_t nodeId = mMesh.getNodeID();
  nodeId = (nodeId % MAX_NODE_ID) + 1;

  // Node ID 0 and 1 are reserved for Master
  if (0 == nodeId) {
    nodeId = 2;
  }
  if (1 == nodeId) {
    nodeId = 2;
  }

  mMesh.setNodeID(nodeId);
}
}

#endif