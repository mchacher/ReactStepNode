#include "react_scheduler.h"
#include "ArduinoLog.h"

/*
 * @brief Constructor for the Task class.
 *
 * @param interval The time interval (in milliseconds) at which the task should execute.
 * @param callback The function pointer to the task's callback function.
 */
Task::Task(unsigned long interval, void (*callback)())
{
    this->interval = interval;
    this->callback = callback;
    previousRun = 0;
    enabled = false; // Task is enabled by default
}

/**
 * @brief Enable the task for execution.
 */
void Task::enable()
{
    enabled = true;
}

/**
 * @brief Disable the task, preventing it from executing.
 */
void Task::disable()
{
    enabled = false;
}

/**
 * @brief Check if the task is currently enabled.
 *
 * @return true if the task is enabled, false otherwise.
 */
bool Task::isEnabled()
{
    return enabled;
}

/**
 * @brief Execute the task if it's enabled and the specified interval has elapsed.
 */
void Task::execute()
{
    if (enabled)
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousRun >= interval)
        {
            previousRun = currentMillis;
            callback();
        }
    }
}

/**
 * @brief Constructor for the ReactScheduler class.
 */
ReactScheduler::ReactScheduler()
{
    taskCount = 0;
}

/**
 * @brief Initialize the scheduler by setting the previousMillis to the current time.
 */
void ReactScheduler::init()
{
    previousMillis = millis();
}

/**
 * @brief Add a task to the scheduler.
 *
 * @param task The Task object to add to the scheduler.
 */
void ReactScheduler::addTask(Task &task)
{
    if (taskCount < MAX_TASKS)
    { // Ensure we don't exceed the maximum number of tasks
        tasks[taskCount] = &task;
        taskCount++;
    }
    else
    {
        Log.noticeln(F("[%s] React Scheduler cannot handle more than %i tasks"), __func__, MAX_TASKS);
        while(true);
    }
}

/**
 * @brief Execute all enabled tasks in the scheduler if their intervals have elapsed.
 */
void ReactScheduler::execute()
{
    for (int i = 0; i < taskCount; i++)
    {
        tasks[i]->execute();
    }
}
