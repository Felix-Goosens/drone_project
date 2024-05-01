#pragma once
#include <Arduino.h>

struct timing_struct{
	unsigned long time;
	unsigned long period;
};

class timing_class{
	uint32_t amount;
	struct timing_struct* times;

public:
	timing_class();

	uint32_t add_timing(uint32_t period);
	bool is_time(uint32_t id);
	void reset(uint32_t id);
};
