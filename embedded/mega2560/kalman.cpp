#include "kalman.h"

void kalman_class::init(float measurement_std, float prediction_std){
    this->measurement_std = measurement_std;
    this->prediction_std = prediction_std;
}

float kalman_class::predict(float measurement, float prediction, float dtime){
    this->measurement_uncertainty = this->measurement_uncertainty + dtime * dtime * this->measurement_std * this->measurement_std;

    float gain = this->measurement_uncertainty / (this->measurement_uncertainty + this->prediction_std * this->prediction_std);

    return measurement + gain * (prediction - measurement);
}