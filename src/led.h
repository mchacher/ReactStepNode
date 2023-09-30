#ifndef LED_H
#define LED_H

typedef enum
{
    EFFECT_RAINBOW,
    EFFECT_NONE
} LED_EFFECT;

#define COLOR_WHITE 0xFFFFFF
#define COLOR_BLACK 0x000000
#define COLOR_RED 0xFF0000
#define COLOR_GREEN 0x008000
#define COLOR_BLUE 0x0000FF
#define COLOR_PINK 0xFF20CC     
#define COLOR_ORANGE 0xFFA500
#define COLOR_YELLOW 0xFFFF00


void led_setup();
void led_task();
void led_set_color(uint32_t color);
void led_set_effect(LED_EFFECT effect);

#endif