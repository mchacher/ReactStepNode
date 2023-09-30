#ifndef REACT_SCHEDULER_H
#define REACT_SCHEDULER_H

#include <Arduino.h>


class Task {
public:
  Task(unsigned long interval, void (*callback)());

  // Function to enable the task
  void enable();

  // Function to disable the task
  void disable();

  // Function to check if the task is enabled
  bool isEnabled();

  // Function to execute the task
  void execute();

private:
  unsigned long interval;
  void (*callback)();
  unsigned long previousRun;
  bool enabled;
};

class ReactScheduler {
public:
  ReactScheduler();

  // Function to initialize the scheduler
  void init();

  // Function to add a task to the scheduler
  void addTask(Task& task);

  // Function to execute tasks in the scheduler
  void execute();

private:
  unsigned long previousMillis;
  Task* tasks[10]; // You can adjust the maximum number of tasks as needed
  int taskCount;
};

#endif
