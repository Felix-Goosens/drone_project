#include <serial_comm.h>
#include "status_class.h"
#include <Adafruit_BMP280.h>
#include <device_status.h>
#include <timing.h>
#include "command_parser.h"

serial_comm esp_comm(&Serial1);

status_class status(&esp_comm);

device_status_class dev_status(24,28,26);

timing_class timing;
size_t heartbeat_t;
size_t status_t;

command_parser cmd_parser;

void setup() {
	Serial.begin(9600);
	Serial1.begin(9600);
	if(status.init() != 0){
		dev_status.error();
	}
	status_t = timing.add_timing(200);
	heartbeat_t = timing.add_timing(500);
}

void loop() {
	if(timing.is_time(heartbeat_t)){
		Serial.println("Heartbeat");
		dev_status.heartbeat();
	}
	if(timing.is_time(status_t)){
		status.update();
		if(status.send()){
			dev_status.warning();
		}
	}
	if(esp_comm.recv() == 0){
		if(cmd_parser.execute_command(&esp_comm.recv_msg) != 0){
			dev_status.warning();
		}
	}
}
