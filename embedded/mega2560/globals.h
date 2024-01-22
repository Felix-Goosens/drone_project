#pragma once
#include <serial_comm.h>
#include "mpu_dev.h"
#include <Adafruit_BMP280.h>
#include <device_status.h>
#include <timing.h>
#include "ESC.h"

extern const int MOTOR_PIN1;
extern const int MOTOR_PIN2;
extern const int MOTOR_PIN3;
extern const int MOTOR_PIN4;
extern const int LED_VCC_PIN;
extern const int LED_GND_PIN;

extern ESC M1;
extern ESC M2;
extern ESC M3;
extern ESC M4;

extern serial_comm ESP_COMM;
extern mpu_dev_class MPU_DEV;
extern device_status_class DEV_STATUS;
 
extern timing_class TIMING;
