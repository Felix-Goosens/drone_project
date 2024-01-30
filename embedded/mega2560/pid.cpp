#include "pid.h"

double pid_class::proportional(double correction){
    // Not used
    return this->factors.P_factor * correction;
}

double pid_class::integral(double correction){
    // Not used
    if (this->clamped){
        return 0;
    }
    return 0;
}

double pid_class::derivitive(double correction){
    // Not used
    return 0;
}

double pid_class::clamper(double correction, double error){
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

double pid_class::update(double error, double feedback, double dtime){
    double correction;
    error = error - feedback;
    this->measurements_index++;
    this->measurements[this->measurements_index] = error;
    correction = 
        this->measurements[(this->measurements_index-1)%3] + 
            (this->factors.P_factor + 
            this->not_clamped * this->factors.I_factor * dtime + 
            this->factors.D_factor / dtime) * 
        this->measurements[this->measurements_index] +
            (-this->factors.P_factor - 
            2 * this->factors.D_factor / dtime) * 
        this->measurements[(this->measurements_index-1)%3] +
        this->factors.D_factor / dtime * this->measurements[(this->measurements_index-2)%3];
        
    if (this->use_clamper){
        correction = clamper(correction, error);
    }
    return correction;
}
