#include "mega2560.h"

bool STARTUP = false;
const int MPU_SAMPLING_RATE = 200;
const int FC_SAMPLING_RATE = 4;

const int MOTOR_PIN1 = 48;
const int MOTOR_PIN2 = 46;
const int MOTOR_PIN3 = 44;
const int MOTOR_PIN4 = 42;
const int LED_VCC_PIN = 41;
const int LED_GND_PIN = 39;

class ESC M1(MOTOR_PIN1,1000,2000,700);
class ESC M2(MOTOR_PIN2,1000,2000,700);
class ESC M3(MOTOR_PIN3,1000,2000,700);
class ESC M4(MOTOR_PIN4,1000,2000,700);

class serial_comm ESP_COMM;
class mpu_dev_class MPU_DEV;
class device_status_class DEV_STATUS(LED_VCC_PIN,-1,LED_VCC_PIN);

class timing_class TIMING;

class flight_controller_class FC;

void setup() {
	pinMode(LED_GND_PIN, OUTPUT);
	digitalWrite(LED_GND_PIN, LOW);

	Serial.begin(9600);
	Serial1.begin(9600);
	ESP_COMM.init(&Serial1);
	if(!MPU_DEV.init()){
		DEV_STATUS.error('I');
	}
}

void loop() {

	static size_t heartbeat_t = TIMING.add_timing(1000);
	static size_t mpu_collect_t = TIMING.add_timing(1000/MPU_SAMPLING_RATE);
	static size_t fc_update_t = TIMING.add_timing(1000/FC_SAMPLING_RATE);
	static class command_parser cmd_parser;

	if(TIMING.is_time(heartbeat_t)){
#ifdef DEBUG
		Serial.println("Heartbeat");
#endif
		DEV_STATUS.heartbeat();
	}
	if(TIMING.is_time(mpu_collect_t)){
		MPU_DEV.update();
	}
	if(TIMING.is_time(fc_update_t) && STARTUP){
		FC.update();
	}
	if(ESP_COMM.recv() == 0){
#ifdef DEBUG
		Serial.println("Received message");
#endif
		if(cmd_parser.execute_command(&ESP_COMM.recv_msg) != 0){
			// Invalid command
			DEV_STATUS.error('M');
		}
	}
}
