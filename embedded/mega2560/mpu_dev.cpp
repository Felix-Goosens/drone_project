#include "mpu_dev.h"

int fcompare(const void* a, const void* b){
    return (*(float*)a > *(float*)b) - (*(float*)a < *(float*)b);
}

int icompare(const void* a, const void* b){
    return *(int16_t*)a > *(int16_t*)b;
}

void mpu_dev_class::update(){
	this->temp = this->bmp.readTemperature();
	this->pressure = this->bmp.readPressure();

	this->mpu.read_acc();
	this->ax = this->mpu.ax;
	this->ay = this->mpu.ay;
	this->az = this->mpu.az;

	this->mpu.read_gyro();
	this->gx = this->mpu.gx;
	this->gy = this->mpu.gy;
	this->gz = this->mpu.gz;

	this->mpu.read_mag();
	this->mx = this->mpu.mx;
	this->my = this->mpu.my;
	this->mz = this->mpu.mz;
}

int mpu_dev_class::init(){
	if(!this->bmp.begin()){
		return -1;
	}
	this->bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode.
		Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling
		Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
		Adafruit_BMP280::FILTER_X16,      // Filtering.
		Adafruit_BMP280::STANDBY_MS_500); // Standby time
	if(this->mpu.init()){
		return -1;
	}
	return 0;
}