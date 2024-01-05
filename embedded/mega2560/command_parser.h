#pragma once
#include <Arduino.h>
#include <serial_comm.h>
#include "ESC.h"

class command_parser {
	ESC* motor_pin1, *motor_pin2, *motor_pin3, *motor_pin4;
public:
	command_parser(ESC* motor_pin1, ESC* motor_pin2, ESC* motor_pin3, ESC* motor_pin4);
	int execute_command(struct msg_struct* command);
};
