#include "command_parser.h"

#define SEND_DEBUG(var) \
	ESP_COMM.append_msg(#var "\x00", strlen(#var)+1); \
	ESP_COMM.append_msg((char*)&var,4);

#define SEND_VAR(var) \
	ESP_COMM.append_msg((char*)&var,sizeof(var)); \

int command_parser::execute_command(struct msg_struct* command){
	static int state = 0;
	uint8_t msg_type;
	if(state == 0){
		if(ESP_COMM.recv()){
			state = 1;
		}
		return state;
	}
	else if(state == 2){
		if(ESP_COMM.send(command->type)){
			state = 0;
		}
		return state;
	}

	uint16_t* motor_values = 0;
	float* magnetic_declination;
	int *value_range;
	struct pid_factors_struct PID_factors[4];

	led_off(static_cast<int>(LED_PINS::bad_message));
	switch(command->type){
		case(CMD_TYPE_DEBUG):
			// Used for debugging purposes
			//SEND_DEBUG(MPU_DEV.radps.x);

			msg_type = MSG_TYPE_DEBUG;
			break;
		case(CMD_TYPE_MOTORS):
			motor_values = (uint16_t*)command->msg;

			M1.speed(motor_values[0]);
			M2.speed(motor_values[1]);
			M3.speed(motor_values[2]);
			M4.speed(motor_values[3]);

			msg_type = MSG_TYPE_MOTORS;
			break;
		case(CMD_TYPE_STATUS):
			ESP_COMM.append_msg((char*)&FC.pitch,sizeof(FC.pitch));
			ESP_COMM.append_msg((char*)&FC.roll,sizeof(FC.roll));
			ESP_COMM.append_msg((char*)&FC.yaw,sizeof(FC.yaw));

			ESP_COMM.append_msg((char*)&FC.height_correction,sizeof(FC.height_correction));
			ESP_COMM.append_msg((char*)&FC.yaw_correction,sizeof(FC.yaw_correction));
			ESP_COMM.append_msg((char*)&FC.roll_correction,sizeof(FC.roll_correction));
			ESP_COMM.append_msg((char*)&FC.pitch_correction,sizeof(FC.pitch_correction));

			ESP_COMM.append_msg((char*)&FC.target_height,sizeof(FC.target_height));
			ESP_COMM.append_msg((char*)&FC.height,sizeof(FC.height));

			msg_type = MSG_TYPE_STATUS;
			break;
		case(CMD_TYPE_CAL):
			magnetic_declination = (float*)command->msg;
			MPU_DEV.calibrate(*magnetic_declination);

			msg_type = MSG_TYPE_CALIBRATE;
			break;
		case(CMD_TYPE_ARM):
			M1.arm();
			M2.arm();
			M3.arm();
			M4.arm();

			msg_type = MSG_TYPE_ARM;
			break;
		case(CMD_TYPE_STARTUP):
			value_range = (int*)command->msg;
			PID_factors[0] = ((struct pid_factors_struct *)&command->msg[4])[0];
			PID_factors[1] = ((struct pid_factors_struct *)&command->msg[4])[1];
			PID_factors[2] = ((struct pid_factors_struct *)&command->msg[4])[2];
			PID_factors[3] = ((struct pid_factors_struct *)&command->msg[4])[3];

			FC.init(PID_factors[0],PID_factors[1],PID_factors[2],PID_factors[3],value_range[0],value_range[1]);
			STARTUP = true;

			led_on(static_cast<int>(LED_PINS::armed));
			led_off(static_cast<int>(LED_PINS::emergency_halt));

			msg_type = MSG_TYPE_STARTUP;
			break;
		case(CMD_TYPE_SHUTDOWN):
			STARTUP = false;

			led_off(static_cast<int>(LED_PINS::armed));

			msg_type = MSG_TYPE_SHUTDOWN;
			break;
		case(CMD_TYPE_HEIGHT):
			FC.target_height += *(float*)(command->msg);
			
			msg_type = MSG_TYPE_HEIGHT;
			break;
		default:
			DBGPRINT("Wrong message type")
			msg_type = MSG_TYPE_ERROR;
			led_on(static_cast<int>(LED_PINS::bad_message));
			break;
	}
	state = 2;
	return state;
}