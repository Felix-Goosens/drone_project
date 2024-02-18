#include "math.h"

struct pid_factors_struct{
    float P_factor = 0;
    float I_factor = 0;
    float D_factor = 0;
};

class pid_class{
    float proportional(float correction);
    float integral(float error, float dtime);
    float derivative(float error, float dtime);
    float clamper(float correction, float error);
    bool clamped = false;
    bool not_clamped = true;
    float prev_integral_correction = 0;
    float prev_error = 0;
public:
    float update(float target, float feedback, float dtime);
    struct pid_factors_struct factors;
    bool use_clamper = false;
    float max_val, min_val;
    int get_output();
};