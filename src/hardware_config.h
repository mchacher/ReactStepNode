#ifndef CONFIG_H
#define CONFIG_H

#define REACT_MESH              0 // set to 1 to activate RADIO
#define LOCAL_COMMAND_BUTTONS   1 // set to 1 to activate PLAY/PAUSE, STOP and CUSTOM Buttons
#define DIGITAL_FOOT_SENSOR     1 // set to 1 for digital foot sensor (typically on mock up)

// Raspberry RP 2040 configuration
#ifdef RP2040

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

// Atemga 328 P configuration
#ifdef ATMEGA328P

// TM 1637 display
#define PIN_TM_1637_CLK     2
#define PIN_TM_1637_DIO     3

// Foot Sensor
#define PIN_PB_FOOT_LEFT    14
#define PIN_PB_FOOT_RIGHT   15

// Microphone
#define PIN_MIC             A2

// push buttons
#define PIN_PB_SET          5
#define PIN_PB_STOP         9
#define PIN_PB_CUSTOM       6
#define PIN_PB_PLAY_PAUSE   10

// #leds are in the strip
#define PIN_LED_DATA        4
#define NUM_LEDS            3

#endif

#endif // CONFIG_H