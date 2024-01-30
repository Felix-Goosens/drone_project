#include "math.h"

struct pid_factors_struct{
    double P_factor = 0;
    double I_factor = 0;
    double D_factor = 0;
};

class pid_class{
    double proportional(double correction);
    double integral(double correction);
    double derivitive(double correction);
    double clamper(double correction, double error);
    bool clamped = false;
    bool not_clamped = true;
    double measurements[3];
    int measurements_index = 0;
public:
    double update(double error, double feedback, double dtime);
    struct pid_factors_struct factors;
    bool use_clamper = false;
    double max_val, min_val;
    int get_output();
};