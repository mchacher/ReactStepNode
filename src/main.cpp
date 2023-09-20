#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <ArduinoLog.h>
#include <TaskScheduler.h>
#include <printf.h>
#include "rf.h"
#include "led.h"
#include "runtime.h"

// Tasks
Task taskLED(50, TASK_FOREVER, &led_task);
// Task taskRF(10, TASK_FOREVER, &taskCallbackRF);
Task taskRuntime(RUNTIME_CYCLE, TASK_FOREVER, &runtime_task);

Scheduler runner;


void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.notice(F("Starting React Step Node with ID: %d" CR), NODE_ID);
  // setupRF();
  runtime_setup();
  // initialize scheduler
  Log.notice(F("Initialized scheduler" CR));
  runner.init();
  led_setup();
  runner.addTask(taskLED);
  taskLED.enable();
  Log.notice(F("--- taskLED: added and enabled" CR));
  // runner.addTask(taskRF);
  // taskRF.enable();
  // Serial.println("--- taskRF: added and enabled");
  runner.addTask(taskRuntime);
  taskRuntime.enable();
  Log.notice(F("--- taskRuntime: added and enabled" CR));
}

void loop()
{
  runner.execute();
}