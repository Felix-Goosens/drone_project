#include "command_parser.h"
#include "ESC.h"

command_parser::command_parser(ESC* motor_pin1, ESC* motor_pin2, ESC* motor_pin3, ESC* motor_pin4) : 
	motor_pin1(motor_pin1), motor_pin2(motor_pin2), motor_pin3(motor_pin3), motor_pin4(motor_pin4){
}

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
			this->motor_pin1->speed(motor_values[0]);
			this->motor_pin2->speed(motor_values[1]);
			this->motor_pin3->speed(motor_values[2]);
			this->motor_pin4->speed(motor_values[3]);

			break;
		default:
			Serial.println("Unknown message type");
			return -1;
	}
	return 0;
}