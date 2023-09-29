#include <Arduino.h>
#include "rf.h"
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <ArduinoLog.h>


// Configure nrf24l01 CE and CS pins
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

uint32_t displayTimer = 0;
static uint8_t node_id = 2;

uint8_t rf_get_node_id()
{
  return node_id;
}

void rf_increment_node_id()
{
  node_id = (node_id % MAX_NODE_ID) + 1;
}

void rf_task()
{
  mesh.update();

  // Send to the master node every second
  if (millis() - displayTimer >= 1000)
  {
    displayTimer = millis();

    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&displayTimer, 'M', sizeof(displayTimer)))
    {

      // If a write fails, check connectivity to the mesh network
      if (!mesh.checkConnection())
      {
        // refresh the network address
        Log.noticeln("Renewing Address");
        if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS)
        {
          // If address renewal fails, reconfigure the radio and restart the mesh
          // This allows recovery from most if not all radio errors
          mesh.begin();
        }
      }
      else
      {
        Log.noticeln("Send fail, Test OK");
      }
    }
    else
    {
      Log.noticeln(F("Send OK: %lu"),displayTimer);
    }
  }

  while (network.available())
  {
    RF24NetworkHeader header;
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Log.notice("Received packet #%i", payload.counter);
    Log.noticeln(F(" at %i"),payload.ms);
  }
}

void rf_setup()
{
  // Set the nodeID manually
  mesh.setNodeID(node_id);

  // Set the PA Level to MIN and disable LNA for testing & power supply related issues
  radio.begin();
  radio.setPALevel(RF24_PA_MIN, 0);

  // Connect to the mesh
  Log.noticeln(F("Connecting to the mesh..."));
  if (!mesh.begin())
  {
    if (radio.isChipConnected())
    {
      do
      {
        // mesh.renewAddress() will return MESH_DEFAULT_ADDRESS on failure to connect
        Log.noticeln(F("Could not connect to network.\nConnecting to the mesh..."));
      } while (mesh.renewAddress() == MESH_DEFAULT_ADDRESS);
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
}