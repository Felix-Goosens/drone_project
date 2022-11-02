#include <device_status.h>

device_status_class::device_status_class(int heartbeat_pin, int warning_pin, int error_pin) : heartbeat_pin(heartbeat_pin), warning_pin(warning_pin), error_pin(error_pin){
	if(heartbeat_pin != -1){
		pinMode(heartbeat_pin, OUTPUT);
		digitalWrite(heartbeat_pin, LOW);
		this->heartbeat_on = false;
	}
	if(warning_pin != -1){
		pinMode(warning_pin, OUTPUT);
		digitalWrite(warning_pin, LOW);
	}
	if(error_pin != -1){
		pinMode(error_pin, OUTPUT);
		digitalWrite(error_pin, LOW);
	}
}

void device_status_class::heartbeat(){
	if(this->heartbeat_on){
		digitalWrite(this->heartbeat_pin, LOW);
		this->heartbeat_on = false;
	}
	else{
		digitalWrite(this->heartbeat_pin, HIGH);
		this->heartbeat_on = true;
	}
}

void device_status_class::warning(){
	digitalWrite(this->warning_pin, HIGH);
	delay(10);
	digitalWrite(this->warning_pin, LOW);
}

void device_status_class::error(){
	digitalWrite(this->error_pin, HIGH);
	while(1){}
}
