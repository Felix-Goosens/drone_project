#include <timing.h>

timing_class::timing_class(){
	this->amount = 0;
	this->times = 0;
}

uint32_t timing_class::add_timing(uint32_t period){
	this->amount++;

	this->times = (struct timing_struct*)realloc(this->times,this->amount*sizeof(struct timing_struct));

	this->times[this->amount-1].time = millis();
	this->times[this->amount-1].period = period;

	return this->amount-1;
}

bool timing_class::is_time(uint32_t id){
	struct timing_struct* t = &this->times[id];
	if(millis() - t->time >= t->period){
		t->time = millis();
		return true;
	}
	return false;
}

void timing_class::reset(uint32_t id){
	struct timing_struct* t = &this->times[id];
	t->time = millis();
}