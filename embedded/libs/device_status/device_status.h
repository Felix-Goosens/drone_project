#pragma once
#include <Arduino.h>

class device_status_class{
public:
	device_status_class(int heartbeat_pin, int warning_pin, int error_pin);

	bool heartbeat_on;

	int heartbeat_pin;

	int warning_pin;

	int error_pin;

	void heartbeat();

	void warning();

	void error(char);

};
