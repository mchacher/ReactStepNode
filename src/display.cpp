#include "Arduino.h"
#include "display.h"
#include <TM1637TinyDisplay.h>
#include <ArduinoLog.h>
#include "hardware_config.h"
#include "state_machine.h"

TM1637TinyDisplay display(PIN_TM_1637_CLK, PIN_TM_1637_DIO);

static const char *msg = MSG_HELLO;
static unsigned long expiration_time = 0;
static bool task_active = false;

/**
 * @brief Initialize the display and set its brightness.
 */
void display_setup()
{
    Log.noticeln(F("display: setup"));
    display.begin();
    display.setBrightness(5);
}

/**
 * @brief Display a message on the TM1637 display.
 *
 * @param message The message to display.
 */
void display_message(const char *message)
{
    if (task_active)
    {
        display.stopAnimation();
    }
    display.clear();
    msg = message;
    display.startStringScroll_P(msg, 300);
    expiration_time = 0; // Message will be displayed indefinitely
    task_active = true;
}

/**
 * @brief Display a message on the TM1637 display for a specified duration.
 *
 * @param message The message to display.
 * @param duration The duration in seconds for which the message should be displayed.
 */
void display_message(const char *message, uint8_t duration)
{
    if (task_active)
    {
        display.stopAnimation();
    }
    display.clear();
    msg = message;
    display.startStringScroll_P(msg, 300);
    expiration_time = millis() + duration * 1000;
    task_active = true;
}

/**
 * @brief Display a message on the TM1637 display for a specified duration.
 *
 * @param message The message to display.
 * @param duration The duration in seconds for which the message should be displayed.
 */
void display_number(uint16_t num)
{
    if (task_active)
    {

        display.stopAnimation();
        display.clear();
        msg = MSG_NONE;
        task_active = false;
    }
    display.showNumber((int)num, true);
}

/**
 * @brief Perform display tasks, including message scrolling and clearing expired messages.
 */
void display_task()
{
    if (task_active)
    {
        bool isAnimationRunning = display.Animate();

        if (!isAnimationRunning)
        {
            Log.verbose(F("display: Animation not running"));
            display.startStringScroll_P(msg, 300);
        }

        if (expiration_time != 0 && millis() > expiration_time)
        {
            display_clear();
        }
    }
}

/**
 * @brief Clear the display and reset the current message.
 */
void display_clear()
{
    display.clear();
    msg = MSG_NONE;
    task_active = false;
}
