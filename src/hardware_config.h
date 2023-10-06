#ifndef CONFIG_H
#define CONFIG_H

#define REACT_MESH              1 // set to 1 to activate RADIO
m // set to 1 to activate PLAY/PAUSE, STOP and CUSTOM Buttons
#define DIGITAL_FOOT_SENSOR     1 // set to 1 for digital foot sensor (typically on mock up)

// TM 1637 display
#define PIN_TM_1637_CLK     2
#define PIN_TM_1637_DIO     3

// Foot Sensor
#define PIN_PB_FOOT_LEFT    14
#define PIN_PB_FOOT_RIGHT   15

// Microphone
#define MIC_PIN             A2

// push buttons
#define PIN_PB_SET          5
#define PIN_PB_STOP         9
#define PIN_PB_CUSTOM       6
#define PIN_PB_PLAY_PAUSE   10

// #leds are in the strip
#define PIN_LED_DATA        4
#define NUM_LEDS            3


#endif