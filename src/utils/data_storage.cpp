#include "Arduino.h"
#include "data_storage.h"
#include "EEPROM.h"

void data_storage_setup()
{
    EEPROM.begin(256);
}

void data_storage_save(PersistentData data, uint8_t value)
{
    int address;
    switch (data)
    {
    case PersistentData::NODE_ID:
        address = static_cast<int>(PersistentData::NODE_ID);
        break;
    }
    EEPROM.write(address, value);
    EEPROM.commit();
}

void data_storage_load(PersistentData data, uint8_t& value)
{
    int address;
    switch (data)
    {
    case PersistentData::NODE_ID:
        address = static_cast<int>(PersistentData::NODE_ID);
        break;
        // Add more cases for other enum values if needed
    }
    value = EEPROM.read(address);
}