#include "Arduino.h"
#include "ArduinoLog.h"
#include "mic.h"
#include "hardware_config.h"

#ifdef ATMEGA328P
#define DC_OFFSET 256 // DC offset  [0]
#define NOISE 25      // Noise/hum/interference [10]
#endif

#ifdef RP2040
#define DC_OFFSET 256
#define NOISE 140
#endif


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
