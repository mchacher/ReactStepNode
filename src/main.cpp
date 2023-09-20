#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <ArduinoLog.h>
#include <TaskScheduler.h>
#include <printf.h>
#include "rf.h"
#include "led.h"
#include "runtime.h"
#include "event_registry.h"
#include "foot_sensor.h"

// Tasks
Task task_led(50, TASK_FOREVER, &led_task);
// Task taskRF(10, TASK_FOREVER, &taskCallbackRF);
Task task_runtime(RUNTIME_CYCLE, TASK_FOREVER, &runtime_task);
Task task_event_registry(100, TASK_FOREVER, &event_registry_task);
Task task_foot_sensor(20, TASK_FOREVER, &foot_sensor_task);
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
  runner.addTask(task_led);
  task_led.enable();
  Log.notice(F("--- task_led: added and enabled" CR));
  // runner.addTask(taskRF);
  // taskRF.enable();
  // Serial.println("--- taskRF: added and enabled");
  event_registry_setup();
  runner.addTask(task_event_registry);
  task_event_registry.enable();
  foot_sensor_setup();
  runner.addTask(task_foot_sensor);
  task_foot_sensor.enable();
  runner.addTask(task_runtime);
  task_runtime.enable();
  Log.notice(F("--- task_runtime: added and enabled" CR));
}

void loop()
{
  runner.execute();
}