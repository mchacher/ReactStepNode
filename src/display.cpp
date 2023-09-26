#include "Arduino.h"
#include "display.h"
#include <TM1637TinyDisplay.h>
#include <ArduinoLog.h>
#include "hardware_config.h"
#include "state_machine.h"
#include "queue.h"

TM1637TinyDisplay display(PIN_TM_1637_CLK, PIN_TM_1637_DIO);

static const char *msg = MSG_HELLO;
static int number;
static unsigned long expiration_time = 0;
static bool task_active = false;
static bool blink = false;
static bool show_number = true;

// Define the maximum queue size
const int MAX_QUEUE_SIZE = 10;
// Create a Queue instance for Messages
Queue<QueuedMessage, MAX_QUEUE_SIZE> messages_queue;

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
    if (duration == 0)
    {
        expiration_time = 0;
    }
    else
    {
        expiration_time = millis() + duration * 1000;
    }

    task_active = true;
}

void display_push_message_to_queue(const char *message, uint8_t duration)
{
    QueuedMessage qmsg;
    Log.noticeln(F("display: pushing message to queue"));
    qmsg.msg = message;
    qmsg.duration = duration;
    messages_queue.push(qmsg);
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
    number = (int)num;
    show_number = true;
    if (blink == false)
    {
        display.showNumber(number, true);
    }
}

/**
 * @brief Enable / Disable blinking of display (only for numbers)
 *
 * @param state if True
 */
void display_blink_numbers(bool state)
{
    blink = state;
}

/**
 * @brief Perform display tasks, including message scrolling and clearing expired messages.
 */
void display_task()
{
    static unsigned long last_blink = 0;
    unsigned long tick = millis();
    static bool on_state = true;

    if (task_active)
    {
        bool isAnimationRunning = display.Animate();
        if (!isAnimationRunning)
        {
            Log.verbose(F("display: Animation not running"));
            display.startStringScroll_P(msg, 300);
            if (expiration_time != 0 && millis() > expiration_time)
            {
                QueuedMessage qmsg;
                if (messages_queue.pop(qmsg))
                {
                    Log.noticeln(F("display: pop message from queue"));
                    display_message(qmsg.msg, qmsg.duration);
                }
            }
        }
    }
    else if ((blink == true) && (show_number == true))
    {
        if ((tick - last_blink) > 700 && (on_state))
        {
            display_clear();
            on_state = false;
            last_blink = millis();
        }
        else if ((tick - last_blink) > 300 && (!on_state))
        {
            display.showNumber(number, true);
            on_state = true;
            last_blink = millis();
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
