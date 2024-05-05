#pragma once

class kalman_class {
    float measurement_uncertainty = 0.0f;
    float measurement_std = 0.0f;
    float prediction_std = 0.0f;

public:
    void init(float measurement_std, float prediction_std);
    float predict(float measurement, float prediction, float dtime);
};