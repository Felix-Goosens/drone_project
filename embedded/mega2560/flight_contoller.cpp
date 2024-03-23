#include "flight_controller.h"
#include <BasicLinearAlgebra.h>
using namespace BLA;

BLA::Matrix<2,2> F;
BLA::Matrix<2,1> G;
BLA::Matrix<2,2> P;
BLA::Matrix<2,2> Q;
BLA::Matrix<2,1> S;
BLA::Matrix<1,2> H;
BLA::Matrix<2,2> I;
BLA::Matrix<1,1> R;

float kalman(float AnglePitch, float AngleRoll, float AccX, float AccY, float AccZ, float height){
	BLA::Matrix<1,1> Acc;
	BLA::Matrix<2,1> K;
	BLA::Matrix<1,1> L;
	BLA::Matrix<1,1> M;

	float AccZInertial = -sin(AnglePitch*(3.142/180.0)) *
		AccX + cos(AnglePitch*(3.142/180.0)) *
		sin(AngleRoll*(3.142/180.0)) *
		AccY + cos(AnglePitch*(3.142/180.0)) *
		cos(AngleRoll*(3.142/180.0))*AccZ;

	AccZInertial = -(AccZInertial-1)*9.81;

#ifdef DEBUG
    Serial.print("AccZInertial: ");
    Serial.println(AccZInertial);
#endif

	Acc = {AccZInertial};
	S = F*S + G*Acc;

#ifdef DEBUG
    Serial.print("S_2: ");
    Serial.println(S(0,0));
#endif

	P = F*P*~F+Q;
	L = H*P*~H+R;
//	K = P*~H*Invert(L);
    K = P*~H*((float)1.0 / L(0, 0));
	M = {height};
	S = S+K*(M-H*S);
//	VelocityVerticalKalman = S(1, 0);
	P = (I-K*H)*P;

#ifdef DEBUG
    Serial.print("K: ");
    Serial.println(K(0,0),3);
    Serial.println(K(1,0),3);
    Serial.println(1/L(0,0),3);
#endif

	return S(0, 0);
}

void flight_controller_class::init(
        struct pid_factors_struct height_factors,
        struct pid_factors_struct yaw_factors,
        struct pid_factors_struct roll_factors,
        struct pid_factors_struct pitch_factors,
        int min_motor_val, int max_motor_val){

    this->height_pid.factors = height_factors;
    this->height_pid.use_clamper = true;
    this->height_pid.min_val = min_motor_val;
    this->height_pid.max_val = max_motor_val;

    this->yaw_pid.factors = yaw_factors;
    this->yaw_pid.use_clamper = true;
    this->yaw_pid.min_val = min_motor_val;
    this->yaw_pid.max_val = max_motor_val;

    this->roll_pid.factors = roll_factors;
    this->roll_pid.use_clamper = true;
    this->roll_pid.min_val = min_motor_val;
    this->roll_pid.max_val = max_motor_val;

    this->pitch_pid.factors = pitch_factors;
    this->pitch_pid.use_clamper = true;
    this->pitch_pid.min_val = min_motor_val;
    this->pitch_pid.max_val = max_motor_val;

    this->target_height = 0;
    this->height_offset = (MPU_DEV.pressure-101325)/12.013;

//    float TS = 1/MPU_SAMPLING_RATE;
    float TS = 0.250;
    F = {(float)1.0, TS, (float)0.0, (float)1.0};
    G = {(float)0.5 * TS*TS, TS};
    P = {(float)0.0, (float)0.0, (float)0.0, (float)0.0};
    Q = G * ~G*(float)10.0*(float)10.0;
    S = {(float)0.0, (float)0.0};
    H = {(float)1.0, (float)0.0};
    I = {(float)1.0, (float)0.0, (float)0.0, (float)1.0};
    R = {(float)30.0*(float)30.0};

    this->last_update_time = millis();
}

float flight_controller_class::correction_transformation(float correction){
    correction += 1000;
    if(correction < 1000){
        correction = 1000;
    }
    else if(correction > 2000){
        correction = 2000;
    }
    return correction;
}

void flight_controller_class::update(){
    float dtime = millis() - this->last_update_time;

#ifdef DEBUG
    Serial.println("kalman start");

    Serial.print("Baro Height: ");
    Serial.println((MPU_DEV.pressure-101325)/12.013 - this->height_offset,3);

    Serial.print("Pitch: ");
    Serial.println(MPU_DEV.pitch,3);
    Serial.print("roll: ");
    Serial.println(MPU_DEV.roll,3);

    Serial.print("accX: ");
    Serial.println(MPU_DEV.mpu.getAccX(),3);
    Serial.print("accY: ");
    Serial.println(MPU_DEV.mpu.getAccY(),3);
    Serial.print("accZ: ");
    Serial.println(MPU_DEV.mpu.getAccZ(),3);

    Serial.println("S: ");
    Serial.println(S(0,0),3);
    Serial.println(S(1,0),3);

    Serial.println("P: ");

    Serial.print(P(0,0),3);
    Serial.print(", ");
    Serial.println(P(0,1),3);

    Serial.print(P(1,0),3);
    Serial.print(", ");
    Serial.println(P(1,1),3);
#endif
    this->height = kalman(MPU_DEV.pitch, MPU_DEV.roll, MPU_DEV.mpu.getAccX(), MPU_DEV.mpu.getAccY(), MPU_DEV.mpu.getAccZ(), (MPU_DEV.pressure-101325.0)/12.013 - this->height_offset);
#ifdef DEBUG
    Serial.println("kalman end");
    Serial.println(this->height,3);
#endif

    this->height_correction = this->height_pid.update(this->target_height, this->height, dtime);
    this->yaw_correction = this->yaw_pid.update(MPU_DEV.yaw, MPU_DEV.yaw, dtime);
    this->roll_correction = this->roll_pid.update(0, MPU_DEV.roll, dtime);
    this->pitch_correction = this->pitch_pid.update(0, MPU_DEV.pitch, dtime);

    M1.speed(this->correction_transformation(this->height_correction - this->yaw_correction + this->roll_correction - this->pitch_correction));
    M2.speed(this->correction_transformation(this->height_correction + this->yaw_correction - this->roll_correction - this->pitch_correction));
    M3.speed(this->correction_transformation(this->height_correction - this->yaw_correction - this->roll_correction + this->pitch_correction));
    M4.speed(this->correction_transformation(this->height_correction + this->yaw_correction + this->roll_correction + this->pitch_correction));

    this->last_update_time = millis();
}