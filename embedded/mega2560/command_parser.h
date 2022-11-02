#pragma once
#include <Arduino.h>
#include <serial_comm.h>

class command_parser {
public:
	int execute_command(struct msg_struct* command);
};
