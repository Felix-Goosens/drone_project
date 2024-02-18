#include "pid.h"
#include <Arduino.h>

float pid_class::proportional(float error){
    return this->factors.P_factor * error;
}

float pid_class::integral(float error, float dtime){
    float correction = this->prev_integral_correction + this->factors.I_factor * dtime * (error + this->prev_error) / 2;
    if (correction > 200){
        return this->prev_integral_correction;
    }
    else if(correction < -200){
        return this->prev_integral_correction;
    }
    this->prev_integral_correction = correction;
    return correction;
}

float pid_class::derivative(float error, float dtime){
    return this->factors.D_factor * (error - this->prev_error) / dtime;
}

float pid_class::clamper(float correction, float error){
    if (correction > this->max_val){
        if(correction > 0 && error > 0){
            this->clamped = true;
        }
        return this->max_val;
    }
    else if(correction < this->min_val){
        if(correction < 0 && error < 0){
            this->clamped = true;
        }
        return this->min_val;
    }
    else{
        this->clamped = false;
        return correction;
    }
}

float pid_class::update(float target, float feedback, float dtime){
    float error = target - feedback;
    float correction = this->proportional(error) + this->integral(error,dtime) + this->derivative(error,dtime);
    this->prev_error = error;
    return correction;
}
