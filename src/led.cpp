#include <Arduino.h>
#include "led.h"
#include <FastLED.h>


// #leds are in the strip
#define NUM_LEDS 1

// Data Pin for WS2812
#define DATA_PIN 4

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

uint32_t current_color = 0xFFFFFF; // black

void led_set_color(uint32_t color)
{
  current_color = color;
}

void led_task()
{
  //static CRGB color = CRGB::Blue;

  // if (color == CRGB::Blue)
  // {
  //   color = CRGB::Red;
  // }
  // else
  // {
  //   color = CRGB::Blue;
  // }
  fill_solid(leds, NUM_LEDS, current_color);
  FastLED.show();
}



void led_setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}