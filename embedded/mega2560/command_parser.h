#pragma once
#include <Arduino.h>
#include <serial_comm.h>
#include "ESC.h"
#include "globals.h"

#define CMD_TYPE_DEBUG MSG_TYPE_DEBUG
#define CMD_TYPE_STATUS MSG_TYPE_STATUS
#define CMD_TYPE_MOTORS MSG_TYPE_MOTORS
#define CMD_TYPE_POS MSG_TYPE_STATUS
#define CMD_TYPE_CAL MSG_TYPE_CALIBRATE
#define CMD_TYPE_ARM MSG_TYPE_ARM
#define CMD_TYPE_STARTUP MSG_TYPE_STARTUP
#define CMD_TYPE_SHUTDOWN MSG_TYPE_SHUTDOWN
#define CMD_TYPE_HEIGHT MSG_TYPE_HEIGHT
#define CMD_TYPE_PID MSG_TYPE_PID

class command_parser {
public:
	int execute_command(struct msg_struct* command);
};