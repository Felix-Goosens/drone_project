#pragma once
#include <serial_comm.h>
#include <Adafruit_BMP280.h>
#include <math.h>
#include "mpu9250.h"

struct vec_struct {
	float x = 0;
	float y = 0;
	float z = 0;
};

class mpu_dev_class{
public:
	int init();

	Adafruit_BMP280 bmp;
	bfs::Mpu9250 imu;

	struct vec_struct radps;
	struct vec_struct radps_offsets;
	struct vec_struct mps2;
	struct vec_struct mps2_offsets;

	float temp, pressure, altitude;
	
	void calibrate(float);
	void update();
	int send();

};