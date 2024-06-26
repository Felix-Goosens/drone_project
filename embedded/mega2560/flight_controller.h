#pragma once
#include <Arduino.h>
#include "kalman.h"
#include "globals.h"
#include "pid.h"

class flight_controller_class{
    float last_update_time;
    class kalman_class kalman_roll, kalman_pitch, kalman_height;
    float correction_transformation(float correction);
public:
    int min_motor_val;
    int max_motor_val;
    class pid_class height_pid, yaw_pid, roll_pid, pitch_pid;

    float height_correction, yaw_correction, roll_correction, pitch_correction;

    float target_height, height, height_offset;
    
	float roll = 0;
    float pitch = 0;
    float yaw = 0;

    void init(
        struct pid_factors_struct height_factors,
        struct pid_factors_struct yaw_factors,
        struct pid_factors_struct roll_factors,
        struct pid_factors_struct pitch_factors,
        int min_motor_val, int max_motor_val
        );
    void update();
};