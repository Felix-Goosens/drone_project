#pragma once
#include <Arduino.h>
#include <serial_comm.h>
#include "ESC.h"
#include "globals.h"

#define CMD_TYPE_DEBUG MSG_TYPE_DEBUG
#define CMD_TYPE_STATUS MSG_TYPE_STATUS
#define CMD_TYPE_MOTORS MSG_TYPE_MOTORS
#define CMD_TYPE_POS MSG_TYPE_STATUS

class command_parser {
public:
	int execute_command(struct msg_struct* command);
};