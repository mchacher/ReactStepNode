#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

// TM 1637 display
#define PIN_TM_1637_CLK     11
#define PIN_TM_1637_DIO     10

// Foot Sensor
#define PIN_PB_FOOT_LEFT    29  // GPIO or ADC input
#define PIN_PB_FOOT_RIGHT   28  // GPIO or ADC input

// Microphone
#define PIN_MIC             27  // GPIO or ADC input

// push buttons
#define PIN_PB_CUSTOM       15
#define PIN_PB_STOP         14
#define PIN_PB_PLAY_PAUSE   13
#define PIN_PB_SET          12

// #leds are in the strip
#define PIN_LED_DATA        9
#define NUM_LEDS            3

// RADIO
#define PIN_CE              5
#define PIN_CSN             1
#define PIN_MISO            4
#define PIN_MOSI            3
#define PIN_SCK             2

#endif