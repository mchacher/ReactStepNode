#include <Arduino.h>
#include <WS2812.h>
#include "led.h"
#include "ArduinoLog.h"
#include "hardware_config.h"

// use the cRGB struct hsv method
#define USE_HSV

WS2812 strip_led(NUM_LEDS);

uint32_t current_color = 0x000000; // black

LED_EFFECT current_effect = LED_EFFECT::EFFECT_NONE;

/**
 * @brief Set the LED strip to a specific color.
 *
 * @param color The color to set (in CRGB format).
 */
void led_set_color(uint32_t color)
{
  current_color = color;
  current_effect = LED_EFFECT::EFFECT_NONE;
}

void led_set_effect(LED_EFFECT effect)
{
  current_effect = effect;
}

void effect_rainbow()
{
  static byte t;
  for (int i = 0; i < NUM_LEDS; i++)
  {
    byte hue = NUM_LEDS + t;
    strip_led.setHSV(i, hue, 255, 255);
  }
  t++;
}

/**
 * @brief Update the LED strip with the current color.
 */
void led_task()
{
  if (current_effect == LED_EFFECT::EFFECT_NONE)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip_led.setRGB(i, current_color);
    }
  }
  else
  {
    switch (current_effect)
    {
    case LED_EFFECT::EFFECT_NONE:
      break;
    case LED_EFFECT::EFFECT_RAINBOW:
      effect_rainbow();
      break;
    default:
      break;
    }
  }
  strip_led.sync();
}

/**
 * @brief Initialize the strip_led module.
 */
void led_setup()
{
  Log.notice(F("led: setup" CR));
  strip_led.setOutput(PIN_LED_DATA);
  strip_led.setBrightness(64);
}
