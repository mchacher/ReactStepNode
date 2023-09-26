#include <Arduino.h>
#include "led.h"
#include <FastLED.h>
#include "ArduinoLog.h"
#include "hardware_config.h"

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

uint32_t current_color = CRGB::Black; // black

/**
 * @brief Set the LED strip to a specific color.
 *
 * @param color The color to set (in CRGB format).
 */
void led_set_color(uint32_t color)
{
  current_color = color;
}

/**
 * @brief Update the LED strip with the current color.
 */
void led_task()
{
  fill_solid(leds, NUM_LEDS, current_color);
  FastLED.show();
}

/**
 * @brief Initialize the LED module.
 */
void led_setup()
{
  Log.notice(F("led: setup" CR));
  pinMode(LED_BUILTIN, OUTPUT);
  FastLED.addLeds<WS2812B, PIN_LED_DATA, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(20);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}
