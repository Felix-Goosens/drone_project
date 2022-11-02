#pragma once
#include <serial_comm.h>
#include <Adafruit_BMP280.h>
#include <MPU9255.h>

class status_class{
public:
	status_class(class serial_comm* comm);

	int init();

	serial_comm* comm;

	Adafruit_BMP280 bmp;
	MPU9255 mpu;

	float temp, pressure, altitude;

	float ax, ay, az, gx, gy, gz, mx, my, mz;
	
	void update();
	int send();

};