#ifndef FOOT_SENSOR_H
#define FOOT_SENSOR_H
#include "app_config.h"

#if DIGITAL_FOOT_SENSOR == 1

void foot_sensor_setup();
void foot_sensor_task();

#endif
#endif