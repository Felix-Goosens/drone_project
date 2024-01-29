#pragma once
#include <serial_comm.h>
#include <Adafruit_BMP280.h>
#include <MPU9250.h>

class mpu_dev_class{
public:
	int init();

	Adafruit_BMP280 bmp;
	MPU9250 mpu;

	float temp, pressure, altitude;

	float roll, pitch, yaw;
	
	void calibrate(float);
	void update();
	int send();

};