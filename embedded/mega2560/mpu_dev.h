#pragma once
#include <serial_comm.h>
#include <Adafruit_BMP280.h>
#include <MPU9255.h>

class mpu_dev_class{
public:
	int init();

	Adafruit_BMP280 bmp;
	MPU9255 mpu;

	float temp, pressure, altitude;

	int16_t ax, ay, az, gx, gy, gz, mx, my, mz;
	
	void update();
	int send();

};