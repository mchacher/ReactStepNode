#include "Arduino.h"
#include "strip_led.h"
#include "ArduinoLog.h"

#ifdef ATMEGA328P

StripLed::StripLed(uint16_t num_leds) : strip_led(num_leds)
{
}

void StripLed::setRGB(uint16_t index, uint32_t rgb)
{
    strip_led.setRGB(index, rgb);
}

void StripLed::setHSV(uint16_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    strip_led.setHSV(index, hue, sat, val);
}

void StripLed::setPin(uint8_t pin)
{
    strip_led.setOutput(pin);
}

void StripLed::setBrightness(uint8_t brightness)
{
    strip_led.setBrightness(brightness);
}

void StripLed::show()
{
    strip_led.sync();
}

StripLed::~StripLed()
{
}

#endif

#ifdef RP2040

StripLed::StripLed(uint16_t num_leds) : strip_led(num_leds, PIN_LED, NEO_GRB + NEO_KHZ800)
{
}

void StripLed::setRGB(uint16_t index, uint32_t rgb)
{
    strip_led.setPixelColor(index, rgb);
    strip_led.show();
}

void StripLed::setHSV(uint16_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    uint32_t rgb = strip_led.ColorHSV((uint16_t)hue * 257U, sat, val);
    strip_led.setPixelColor(index, rgb);
}

void StripLed::setPin(uint8_t pin)
{
    strip_led.setPin(pin);
}

void StripLed::setBrightness(uint8_t brightness)
{
    strip_led.setBrightness(brightness);
}

void StripLed::show()
{
    strip_led.show();
}

StripLed::~StripLed()
{
}

#endif
