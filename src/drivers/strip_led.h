#ifndef STRIP_LED_H
#define STRIP_LED_H

#include "Adafruit_NeoPixel.h"


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
    Adafruit_NeoPixel strip_led;
};

#endif // STRIP_LED_H