
#ifndef RF_H
#define RF_H

#define MAX_NODE_ID 99
struct payload_t
{
  unsigned long ms;
  unsigned long counter;
};

void rf_setup();
void rf_task();
uint8_t rf_get_node_id();
void rf_increment_node_id();

#endif
