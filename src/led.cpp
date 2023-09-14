#include <Arduino.h>
#include <WS2812.h>
#include "led.h"
#include "ArduinoLog.h"
#include "hardware_config.h"
#include "mic.h"

// use the cRGB struct hsv method
#define USE_HSV

WS2812 strip_led(NUM_LEDS);

uint32_t current_color = 0x000000; // black

LED_EFFECTS current_effect = LED_EFFECTS::EFFECT_NONE;

/**
 * @brief Set the LED strip to a specific color.
 *
 * @param color The color to set (in CRGB format).
 */
void led_set_color(uint32_t color)
{
  current_color = color;
  current_effect = LED_EFFECTS::EFFECT_NONE;
}

void led_set_effect(LED_EFFECTS effect)
{
  current_effect = effect;
}

/**
 * @brief manage rainbow effect on led strip
 *
 */
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

#define SAMPLES 2

/**
 * @brief manage music effect on led strip while getting microphone samples
 *
 */
void effect_music()
{
  static uint32_t mic_sum = 0; // Variable to accumulate mic values
  static int mic_count = 0;    // Counter for mic samples

  uint16_t mic_value = mic_read();
  mic_sum += mic_value;
  mic_count++;

  // Define custom hue values for your desired color order
  uint16_t colorHues[] = { 0, 240, 120, 60, 30, 300, 0 };

  // Check if we've collected SAMPLES samples
  if (mic_count >= SAMPLES)
  {
    // Calculate the average mic value
    uint16_t mic_average = mic_sum / SAMPLES;

    // Map mic_average to the range of color hues
    int hueIndex = map(mic_average, 0, 255, 0, 7);
    uint8_t saturation = 255;
    uint8_t value = 255;

    if (hueIndex == 0)
    {
      saturation = 0;
      value = 0;
    }
    // Get the corresponding hue from the custom array
    byte hue = colorHues[hueIndex];

    // Set all LEDs to the calculated color
    for (int i = 0; i < NUM_LEDS; i++)
    {
      strip_led.setHSV(i, hue, saturation, value);
    }

    // Reset the sum and count for the next set of samples
    mic_sum = 0;
    mic_count = 0;
  }
}

/**
 * @brief Update the LED strip with the current color.
 */
void led_task()
{


  if (current_effect == LED_EFFECTS::EFFECT_NONE)
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
    case LED_EFFECTS::EFFECT_RAINBOW:
      effect_rainbow();
      break;
    case LED_EFFECTS::EFFECT_MUSIC:
      effect_music();
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
