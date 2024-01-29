#include "command_parser.h"

int command_parser::execute_command(struct msg_struct* command){
	uint16_t* motor_values = 0;

	switch(command->type){
#ifdef DEBUG
		case(CMD_TYPE_DEBUG):
			Serial.println(command->msg);
			break;
#endif
		case(CMD_TYPE_MOTORS):
#ifdef DEBUG
			Serial.println("CMD_TYPE_MOTORS");
#endif
			if(command->len != sizeof(uint16_t)*4){
				break;
			}
			motor_values = (uint16_t*)command->msg;
#ifdef DEBUG
			Serial.println("Changing motor settings:");
			Serial.print("Moter 1: ");
			Serial.println(motor_values[0],DEC);
			Serial.print("Moter 2: ");
			Serial.println(motor_values[1],DEC);
			Serial.print("Moter 3: ");
			Serial.println(motor_values[2],DEC);
			Serial.print("Moter 4: ");
			Serial.println(motor_values[3],DEC);
#endif
			M1.speed(motor_values[0]);
			M2.speed(motor_values[1]);
			M3.speed(motor_values[2]);
			M4.speed(motor_values[3]);

			break;
		case(CMD_TYPE_STATUS):
#ifdef DEBUG
			Serial.println("CMD_TYPE_STATUS");
#endif
			ESP_COMM.send_msg.type = MSG_TYPE_STATUS;

			ESP_COMM.append_msg((char*)&MPU_DEV.temp,sizeof(MPU_DEV.temp));
			ESP_COMM.append_msg((char*)&MPU_DEV.pressure,sizeof(MPU_DEV.pressure));
			ESP_COMM.append_msg((char*)&MPU_DEV.altitude,sizeof(MPU_DEV.altitude));

			ESP_COMM.append_msg((char*)&MPU_DEV.pitch,sizeof(MPU_DEV.pitch));
			ESP_COMM.append_msg((char*)&MPU_DEV.roll,sizeof(MPU_DEV.roll));
			ESP_COMM.append_msg((char*)&MPU_DEV.yaw,sizeof(MPU_DEV.yaw));

			ESP_COMM.send();
			break;
		case(CMD_TYPE_CAL):
#ifdef DEBUG
			Serial.println("CMD_TYPE_CAL");
#endif
			if(command->len != sizeof(float)){
				break;
			}
			float* magnetic_declination = (float*)command->msg;
			MPU_DEV.calibrate(*magnetic_declination);

			ESP_COMM.send_msg.type = MSG_TYPE_CALIBRATE;
			ESP_COMM.send();
			break;
		default:
#ifdef DEBUG
			Serial.print("Unknown message type: ");
			Serial.println(command->type,DEC);
#endif
			break;
	}
	return 0;
}