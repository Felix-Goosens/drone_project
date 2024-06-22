#include "mega2560.h"

bool STARTUP = false;
const int MPU_SAMPLING_RATE = 200;
const int FC_SAMPLING_RATE = 4;

//const int MOTOR_PIN1 = 48;
//const int MOTOR_PIN2 = 46;
//const int MOTOR_PIN3 = 44;
//const int MOTOR_PIN4 = 42;

const int MOTOR_PIN1 = 37;
const int MOTOR_PIN2 = 35;
const int MOTOR_PIN3 = 33;
const int MOTOR_PIN4 = 31;

class ESC M1(MOTOR_PIN1,1000,2000,700);
class ESC M2(MOTOR_PIN2,1000,2000,700);
class ESC M3(MOTOR_PIN3,1000,2000,700);
class ESC M4(MOTOR_PIN4,1000,2000,700);

class serial_comm ESP_COMM;
class mpu_dev_class MPU_DEV;

class timing_class TIMING;

class flight_controller_class FC;

void setup() {
	led_init(static_cast<int>(LED_PINS::heartbeat));
	led_init(static_cast<int>(LED_PINS::armed));

	led_init(static_cast<int>(LED_PINS::message));
	led_init(static_cast<int>(LED_PINS::reserved_blue));

	led_init(static_cast<int>(LED_PINS::bad_message));
	led_init(static_cast<int>(LED_PINS::loop_freq));

	led_init(static_cast<int>(LED_PINS::emergency_halt));
	led_init(static_cast<int>(LED_PINS::fault));

	Serial.begin(9600);
	Serial1.begin(9600);
	ESP_COMM.init(&Serial1);
	if(MPU_DEV.init() != 0){
		led_on(static_cast<int>(LED_PINS::fault));
		while(1){}
	}
}

void loop() {
	static size_t heartbeat_t = TIMING.add_timing(1000);
	static size_t mpu_collect_t = TIMING.add_timing(1000/MPU_SAMPLING_RATE);
	static size_t fc_update_t = TIMING.add_timing(1000/FC_SAMPLING_RATE);
	static size_t message_led_t = TIMING.add_timing(100);
	static size_t loop_freq_t = TIMING.add_timing(1000/MPU_SAMPLING_RATE);

	static class command_parser cmd_parser;

	if(TIMING.is_time(loop_freq_t)){
		led_on(static_cast<int>(LED_PINS::loop_freq));
	}
	else{
		led_off(static_cast<int>(LED_PINS::loop_freq));
	}
	TIMING.reset(loop_freq_t);

	if(TIMING.is_time(heartbeat_t)){
		DBGPRINT("heartbeat")
		led_flip(static_cast<int>(LED_PINS::heartbeat));
	}

	if(TIMING.is_time(mpu_collect_t)){
		MPU_DEV.update();
	}

	if(TIMING.is_time(fc_update_t) && STARTUP){
		FC.update();
	}

	if(TIMING.is_time(message_led_t)){
		led_off(static_cast<int>(LED_PINS::message));
	}

	if(cmd_parser.execute_command(&ESP_COMM.recv_msg) == 1){
		DBGPRINT("Parsing command");
		led_on(static_cast<int>(LED_PINS::message));
		TIMING.reset(message_led_t);
	}
}
