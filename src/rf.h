
#ifndef RF_H
#define RF_H

#define NODE_ID 2

struct payload_t
{
  unsigned long ms;
  unsigned long counter;
};

void setupRF();
void taskCallbackRF();

#endif
