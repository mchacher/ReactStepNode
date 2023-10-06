#ifndef LED_H
#define LED_H

typedef enum
{
    EFFECT_RAINBOW = 0x22,
    EFFECT_MUSIC = 0x23,
    EFFECT_NONE = 0
} LED_EFFECTS;

typedef enum {
  COLOR_WHITE   = 0xFFFFFF,
  COLOR_BLACK   = 0x000000,
  COLOR_RED     = 0xFF0000,
  COLOR_GREEN   = 0x008000,
  COLOR_BLUE    = 0x0000FF,
  COLOR_PINK    = 0xFF20CC,
  COLOR_ORANGE  = 0xFFA500,
  COLOR_YELLOW  = 0xFFFF00
} LED_COLORS;


void led_setup();
void led_task();
void led_set_color(uint32_t color);
void led_set_effect(LED_EFFECTS effect);

#endif