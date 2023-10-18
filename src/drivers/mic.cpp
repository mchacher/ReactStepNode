#include "Arduino.h"
#include "ArduinoLog.h"
#include "mic.h"
#include "hardware_config.h"

#define DC_OFFSET 256
#define NOISE 140



uint16_t mic_read()
{
  int n = analogRead(PIN_MIC);
  n = abs(n - DC_OFFSET);
  n = (n <= NOISE) ? 0 : (n - NOISE); // Remove noise/hum
  Log.verboseln("value %i", n);
  return n;
}

void setup_mic()
{
  pinMode(PIN_MIC, INPUT);
}
