#pragma once
#include <serial_comm.h>
#include <Adafruit_BMP280.h>
#include <MPU9255.h>

class mpu_dev_class{
public:
	mpu_dev_class(class serial_comm* comm);

	int init();

	serial_comm* comm;

	Adafruit_BMP280 bmp;
	MPU9255 mpu;

	float temp, pressure, altitude;

	int16_t ax, ay, az, gx, gy, gz, mx, my, mz;
	
	void update();
	int send();

};