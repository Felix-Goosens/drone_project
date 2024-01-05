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

void device_status_class::error(char error_code = 'A'){
	while(1){
		digitalWrite(this->error_pin, LOW);
		delay(3000);

		if(error_code == 'A'){
			digitalWrite(this->error_pin, HIGH);
			delay(1000);
			digitalWrite(this->error_pin, LOW);
			delay(1000);
			digitalWrite(this->error_pin, HIGH);
			delay(250);
		}
		else if(error_code == 'E'){
			digitalWrite(this->error_pin, HIGH);
			delay(250);
		}
		else if(error_code == 'M'){
			digitalWrite(this->error_pin, HIGH);
			delay(1000);
			digitalWrite(this->error_pin, LOW);
			delay(1000);
			digitalWrite(this->error_pin, HIGH);
			delay(1000);
		}
		else if(error_code == 'I'){
			digitalWrite(this->error_pin, HIGH);
			delay(250);
			digitalWrite(this->error_pin, LOW);
			delay(1000);
			digitalWrite(this->error_pin, HIGH);
			delay(250);
		}
	}
}
