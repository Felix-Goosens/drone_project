#include "mpu_dev.h"

int fcompare(const void* a, const void* b){
    return (*(float*)a > *(float*)b) - (*(float*)a < *(float*)b);
}

int icompare(const void* a, const void* b){
    return *(int16_t*)a > *(int16_t*)b;
}

int mpu_dev_class::init(){

	if(!this->bmp.begin()){
		this->bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode.
			Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling
			Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
			Adafruit_BMP280::FILTER_X16,      // Filtering.
			Adafruit_BMP280::STANDBY_MS_500); // Standby time
	}

	Wire.begin();
	Wire.setClock(400000);

	/* I2C bus,  0x68 address */
	this->imu.Config(&Wire, bfs::Mpu9250::I2C_ADDR_PRIM);

	/* Initialize and configure IMU */
	if (!this->imu.Begin()) {
		Serial.println("Error initializing communication with IMU");
		return -1;
	}

	// Sample rate is 1000 / (SRD + 1) = 1000 Hz
	if (!this->imu.ConfigSrd(0)) {
		Serial.println("Error configured SRD");
		return -1;
	}
	// 2.16 is the magnetic declination in Amsterdam
	this->calibrate(2.16);

	return 0;
}

void mpu_dev_class::calibrate(float magnetic_declination){
	struct vec_struct radps_avg;
	struct vec_struct mps2_avg;
	int samples = 1000;
	for(int i=0; i<samples; i++){
		delay(10);
		this->update();
		radps_avg.x += this->radps.x;
		radps_avg.y += this->radps.y;
		radps_avg.z += this->radps.z;

		mps2_avg.x += this->mps2.x;
		mps2_avg.y += this->mps2.y;
		mps2_avg.z += this->mps2.z;
	}
	this->radps_offsets.x = radps_avg.x / samples;
	this->radps_offsets.y = radps_avg.y / samples;
	this->radps_offsets.z = radps_avg.z / samples;

	this->mps2_offsets.x = mps2_avg.x / samples;
	this->mps2_offsets.y = mps2_avg.y / samples;
	this->mps2_offsets.z = mps2_avg.z / samples;
}

void mpu_dev_class::update(){
	if(imu.Read()){
		imu.new_imu_data();
		imu.new_mag_data();

		this->radps.x = imu.gyro_x_radps() - this->radps_offsets.x;
		this->radps.y = imu.gyro_y_radps() - this->radps_offsets.y;
		this->radps.z = imu.gyro_z_radps() - this->radps_offsets.z;

		this->mps2.x = imu.accel_x_mps2() - this->mps2_offsets.x;
		this->mps2.y = imu.accel_y_mps2() - this->mps2_offsets.y;
		this->mps2.z = imu.accel_z_mps2() - this->mps2_offsets.z;
	}
	this->temp = this->bmp.readTemperature();
	this->pressure = this->bmp.readPressure();
}
