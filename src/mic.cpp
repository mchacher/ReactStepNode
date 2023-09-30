#include "Arduino.h"
#include "ArduinoLog.h"
#include "mic.h"
#include "hardware_config.h"


#define DC_OFFSET 256      // DC offset  [0]
#define NOISE 25           // Noise/hum/interference [10]

uint16_t mic_read()
{

  int n = 0;
  uint16_t height = 0;
  n = analogRead(MIC_PIN); // Raw reading from  line in
  //n = abs(n - 256 - DC_OFFSET);    // Center on zero
  n = abs(n - DC_OFFSET);
  n = (n <= NOISE) ? 0 : (n - NOISE); // Remove noise/hum
  return n;
}


void setup_mic()
{
    pinMode(MIC_PIN, INPUT);
}

