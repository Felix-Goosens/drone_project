#include "flight_controller.h"
#include <math.h>

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

    this->kalman_roll.init(5, 5);
    this->kalman_pitch.init(5, 5);
    this->kalman_height.init(0.5, 0.5);

    this->last_update_time = millis() / 1000;
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
//    static float uncertainty_pitch = 0.0f;
//    static float uncertainty_roll = 0.0f;
//    static float uncertainty_height = 0.0f;

//    static float int_pitch_std = 4.0f, acc_pitch_std = 3.0f;
//    static float int_height_std = 4.0f, int_roll_std = 4.0f, acc_roll_std = 3.0f;

    float dtime = millis() / 1000.0f - this->last_update_time;

    // angle based on the integral of radians per second
    float int_pitch = this->pitch + MPU_DEV.radps.x * dtime * 360.0f / (2.0f * M_PI);
    float int_roll = this->roll + MPU_DEV.radps.y * dtime * 360.0f / (2.0f * M_PI);

    // angle based on the acceleration
    float acc_pitch = -atan(
        MPU_DEV.mps2.x / sqrt(
        MPU_DEV.mps2.y*MPU_DEV.mps2.y +
        MPU_DEV.mps2.z*MPU_DEV.mps2.z))*
        1.0f / (M_PI / 180.0f);

    float acc_roll = -atan(
        MPU_DEV.mps2.y / sqrt(
        MPU_DEV.mps2.x*MPU_DEV.mps2.x +
        MPU_DEV.mps2.z*MPU_DEV.mps2.z))*
        1.0f / (M_PI / 180.0f);

    float bar_height = 44330.0f * ( 1.0f - pow(MPU_DEV.pressure / 101325.0f, 1.0f / 5.255f) ) + this->height_offset;

    float acc_height = -MPU_DEV.mps2.x * sin(this->pitch) + MPU_DEV.mps2.y * sin(this->roll) * cos(this->pitch) + MPU_DEV.mps2.z * cos(this->roll)*cos(this->pitch) + 9.81;

    // kalman filter for the angles
    this->pitch = this->kalman_pitch.predict(int_pitch, acc_pitch, dtime);
    this->roll = this->kalman_roll.predict(int_roll, acc_roll, dtime);
    // TODO: Check unit of height
    this->height = this->kalman_height.predict(bar_height, acc_height, dtime);

    float new_height_correction = this->height_pid.update(this->target_height, this->height, dtime);
    float new_yaw_correction = this->yaw_pid.update(0, this->yaw, dtime);
    float new_roll_correction = this->roll_pid.update(0, this->roll, dtime);
    float new_pitch_correction = this->pitch_pid.update(0, this->pitch, dtime);

    // To prevent accidents, the flight controller should be disabled if there is a correction of more than 200 units per second
    if (fabs(new_height_correction - this->height_correction) / dtime > 200.0f ||
        fabs(new_yaw_correction - this->yaw_correction) / dtime > 200.0f ||
        fabs(new_roll_correction - this->roll_correction) / dtime > 200.0f ||
        fabs(new_pitch_correction - this->pitch_correction) / dtime > 200.0f ){
        STARTUP = false;
        new_height_correction = 0;
        new_yaw_correction = 0;
        new_roll_correction = 0;
        new_pitch_correction = 0;

        led_on(static_cast<int>(LED_PINS::emergency_halt));
        led_off(static_cast<int>(LED_PINS::armed));
    }

    this->height_correction = new_height_correction;
    this->yaw_correction = new_yaw_correction;
    this->roll_correction = new_roll_correction;
    this->pitch_correction = new_pitch_correction;

    M1.speed(this->correction_transformation(this->height_correction - this->yaw_correction + this->roll_correction - this->pitch_correction));
    M2.speed(this->correction_transformation(this->height_correction + this->yaw_correction - this->roll_correction - this->pitch_correction));
    M3.speed(this->correction_transformation(this->height_correction - this->yaw_correction - this->roll_correction + this->pitch_correction));
    M4.speed(this->correction_transformation(this->height_correction + this->yaw_correction + this->roll_correction + this->pitch_correction));

    this->last_update_time = millis() / 1000.0f;
}