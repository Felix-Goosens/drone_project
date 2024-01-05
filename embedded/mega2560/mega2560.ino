#include <serial_comm.h>
#include "mpu_dev.h"
#include <Adafruit_BMP280.h>
#include <device_status.h>
#include <timing.h>
#include "command_parser.h"

#include "ESC.h"

const int MOTOR_PIN1 = 48;
const int MOTOR_PIN2 = 46;
const int MOTOR_PIN3 = 44;
const int MOTOR_PIN4 = 42;
const int LED_VCC_PIN = 41;
const int LED_GND_PIN = 39;

ESC m1(MOTOR_PIN1,1000,2000,700);
ESC m2(MOTOR_PIN2,1000,2000,700);
ESC m3(MOTOR_PIN3,1000,2000,700);
ESC m4(MOTOR_PIN4,1000,2000,700);

serial_comm esp_comm;

mpu_dev_class mpu_dev(&esp_comm);

device_status_class dev_status(LED_VCC_PIN,-1,LED_VCC_PIN);

timing_class timing;
size_t heartbeat_t;
size_t mpu_collect_t;

command_parser cmd_parser(&m1,&m2,&m3,&m4);

void setup() {
	pinMode(LED_GND_PIN, OUTPUT);
	digitalWrite(LED_GND_PIN, LOW);

	Serial.begin(9600);
	Serial1.begin(9600);
	esp_comm.init(&Serial1);
	if(mpu_dev.init() != 0){
		dev_status.error('I');
	}

	mpu_collect_t = timing.add_timing(250);
	heartbeat_t = timing.add_timing(1000);

	delay(5000);
	m1.arm();
	m2.arm();
	m3.arm();
	m4.arm();
	delay(5000);

}

void loop() {

	if(timing.is_time(heartbeat_t)){
#ifdef DEBUG
		Serial.println("Heartbeat");
#endif
		dev_status.heartbeat();
	}
	if(timing.is_time(mpu_collect_t)){
		mpu_dev.update();
#ifdef DEBUG
		Serial.println("Update");
#endif
		if(mpu_dev.send()){
			dev_status.error('E');
		}
	}
	if(esp_comm.recv() == 0){
#ifdef DEBUG
		Serial.println("Received message");
#endif
		if(cmd_parser.execute_command(&esp_comm.recv_msg) != 0){
			// Invalid command
			dev_status.error('M');
		}
	}
}
