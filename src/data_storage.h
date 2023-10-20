#ifndef DATA_STORAGE_H

enum class PersistentData {
    NODE_ID = 0
};


void data_storage_setup();
void data_storage_save(PersistentData data, uint8_t value);
void data_storage_load(PersistentData data, uint8_t& value);

#endif