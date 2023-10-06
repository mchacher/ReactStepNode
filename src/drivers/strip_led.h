#ifndef STRIP_LED_H
#define STRIP_LED_H

#include "Arduino.h"
#ifdef ATMEGA328P
#include "ws2812.h"
#endif

#ifdef RP2040
#include "Adafruit_NeoPixel.h"
#endif

class StripLed
{
public:
    StripLed(uint16_t num_led);
    ~StripLed();
    void setPin(uint8_t pin);
    void setBrightness(uint8_t brightness);
    void setRGB(uint16_t index, uint32_t rgb);
    void setHSV(uint16_t index, uint8_t hue, uint8_t sat, uint8_t val); // hue between 0 and 191
    void show();
private:
#ifdef ATMEGA328P
    WS2812 strip_led;
#endif
#ifdef RP2040
    Adafruit_NeoPixel strip_led;
#endif
};

#endif // STRIP_LED_H