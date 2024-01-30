#pragma once
#include <Arduino.h>
#include "globals.h"
#include "pid.h"

class flight_controller_class{
    double last_update_time;
public:
    int min_motor_val;
    int max_motor_val;
    class pid_class height_pid, yaw_pid, roll_pid, pitch_pid;

    int target_height;
    void init(
        struct pid_factors_struct height_factors,
        struct pid_factors_struct yaw_factors,
        struct pid_factors_struct roll_factors,
        struct pid_factors_struct pitch_factors,
        int min_motor_val, int max_motor_val
        );
    void update();
};