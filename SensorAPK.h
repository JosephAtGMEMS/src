#ifndef __GMEMS_Sensor
#define __GMEMS_Sensor

#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "PinNames.h"
#include "basic_types.h"
#include <osdep_api.h>

#include "i2c_api.h"
#include "pinmap.h"

#define MBED_I2C_MTR_SDA    PB_3
#define MBED_I2C_MTR_SCL    PB_2

#define MBED_I2C_BUS_CLK        100000  //hz

extern void SensorWORK(void *p);

#endif