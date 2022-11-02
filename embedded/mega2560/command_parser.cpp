#include "command_parser.h"

int command_parser::execute_command(struct msg_struct* command){
	switch(command->type){
		case(MSG_TYPE_DEBUG):
			Serial.println(command->msg);
			break;
		case(MSG_TYPE_MOTORS):
			if(command->len != sizeof(uint16_t)*4){
				return -1;
			}
			uint16_t* motor_values = (uint16_t*)command->msg;
			Serial.println("Changing motor settings:");
			Serial.print("Moter 1: ");
			Serial.println(motor_values[0],DEC);
			Serial.print("Moter 2: ");
			Serial.println(motor_values[1],DEC);
			Serial.print("Moter 3: ");
			Serial.println(motor_values[2],DEC);
			Serial.print("Moter 4: ");
			Serial.println(motor_values[3],DEC);
			break;
		default:
			Serial.println("Unknown message type");
			return -1;
	}
	return 0;
}