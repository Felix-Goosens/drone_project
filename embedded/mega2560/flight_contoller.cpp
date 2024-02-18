#include "flight_controller.h"

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

    this->target_height = (MPU_DEV.pressure-101325)/12.013;
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
    this->height = (MPU_DEV.pressure-101325)/12.013;

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