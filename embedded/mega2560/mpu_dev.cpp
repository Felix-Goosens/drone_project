#include "mpu_dev.h"

int fcompare(const void* a, const void* b){
    return (*(float*)a > *(float*)b) - (*(float*)a < *(float*)b);
}

int icompare(const void* a, const void* b){
    return *(int16_t*)a > *(int16_t*)b;
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
//	return this->mpu.setup(MPU9250_DEFAULT_ADDRESS);
	this->mpu.setMagneticDeclination(2.16);
	return this->mpu.setup(0x68);
}

void mpu_dev_class::calibrate(float magnetic_declination){
	this->mpu.setMagneticDeclination(magnetic_declination);
    this->mpu.calibrateAccelGyro();
    this->mpu.calibrateMag();
}

void mpu_dev_class::update(){
	this->temp = this->bmp.readTemperature();
	this->pressure = this->bmp.readPressure();

	this->mpu.update();
	this->yaw = this->mpu.getYaw();
	this->pitch = this->mpu.getPitch();
	this->roll = this->mpu.getRoll();
}
