#pragma once
#include <serial_comm.h>
#include <Adafruit_BMP280.h>
#include <device_status.h>
#include <timing.h>
#include "mpu_dev.h"
#include "ESC.h"
#include "flight_controller.h"

extern bool STARTUP;

extern const int MPU_SAMPLING_RATE;
extern const int FC_SAMPLING_RATE;

extern const int MOTOR_PIN1;
extern const int MOTOR_PIN2;
extern const int MOTOR_PIN3;
extern const int MOTOR_PIN4;
extern const int LED_VCC_PIN;
extern const int LED_GND_PIN;

extern class ESC M1;
extern class ESC M2;
extern class ESC M3;
extern class ESC M4;

extern class serial_comm ESP_COMM;
extern class mpu_dev_class MPU_DEV;
extern class device_status_class DEV_STATUS;
 
extern class timing_class TIMING;

extern class flight_controller_class FC;
