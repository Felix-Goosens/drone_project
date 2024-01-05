#include "mpu_dev.h"

mpu_dev_class::mpu_dev_class(class serial_comm* comm) : comm(comm){}

int compare(const void* a, const void* b){
    return (*(float*)a > *(float*)b) - (*(float*)a < *(float*)b);
}

void mpu_dev_class::update(){
	uint32_t rounds = 10;

	float farray1[rounds];
	float farray2[rounds];
	float farray3[rounds];

	for(int i=0;i<rounds;i++){
		farray1[i] = this->bmp.readTemperature();
	}
	qsort(farray1,rounds,sizeof(float),compare);
	this->temp = farray1[rounds/2];

	for(int i=0;i<rounds;i++){
		farray1[i] = this->bmp.readPressure();
	}
	qsort(farray1,rounds,sizeof(float),compare);
	this->pressure = farray1[rounds/2];

	for(int i=0;i<rounds;i++){
		farray1[i] = this->bmp.readAltitude(1013.25);
	}
	qsort(farray1,rounds,sizeof(float),compare);
	this->altitude = farray1[rounds/2];

	for(int i=0;i<rounds;i++){
		this->mpu.read_acc();
		farray1[i] = this->mpu.ax;
		farray2[i] = this->mpu.ay;
		farray3[i] = this->mpu.az;
	}
	qsort(farray1,rounds,sizeof(float),compare);
	qsort(farray2,rounds,sizeof(float),compare);
	qsort(farray3,rounds,sizeof(float),compare);

	this->ax = farray1[rounds/2];
	this->ay = farray2[rounds/2];
	this->az = farray3[rounds/2];

	for(int i=0;i<rounds;i++){
		this->mpu.read_gyro();
		farray1[i] = this->mpu.gx;
		farray2[i] = this->mpu.gy;
		farray3[i] = this->mpu.gz;
	}
	qsort(farray1,rounds,sizeof(float),compare);
	qsort(farray2,rounds,sizeof(float),compare);
	qsort(farray3,rounds,sizeof(float),compare);

	this->gx = farray1[rounds/2];
	this->gy = farray2[rounds/2];
	this->gz = farray3[rounds/2];

	for(int i=0;i<rounds;i++){
		this->mpu.read_mag();
		farray1[i] = this->mpu.mx;
		farray2[i] = this->mpu.my;
		farray3[i] = this->mpu.mz;
	}
	qsort(farray1,rounds,sizeof(float),compare);
	qsort(farray2,rounds,sizeof(float),compare);
	qsort(farray3,rounds,sizeof(float),compare);

	this->mx = farray1[rounds/2];
	this->my = farray2[rounds/2];
	this->mz = farray3[rounds/2];
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

int mpu_dev_class::send(){
	this->comm->send_msg.type = MSG_TYPE_STATUS;

	this->comm->append_msg((char*)&this->temp,sizeof(this->temp));
	this->comm->append_msg((char*)&this->pressure,sizeof(this->pressure));
	this->comm->append_msg((char*)&this->altitude,sizeof(this->altitude));

	this->comm->append_msg((char*)&this->ax,sizeof(this->ax));
	this->comm->append_msg((char*)&this->ay,sizeof(this->ay));
	this->comm->append_msg((char*)&this->az,sizeof(this->az));

	this->comm->append_msg((char*)&this->gx,sizeof(this->ax));
	this->comm->append_msg((char*)&this->gy,sizeof(this->ay));
	this->comm->append_msg((char*)&this->gz,sizeof(this->az));

	this->comm->append_msg((char*)&this->mx,sizeof(this->ax));
	this->comm->append_msg((char*)&this->my,sizeof(this->ay));
	this->comm->append_msg((char*)&this->mz,sizeof(this->az));

	return this->comm->send();
}