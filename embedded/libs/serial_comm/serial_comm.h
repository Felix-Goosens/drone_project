#pragma once
#include <Arduino.h>

#define BUFFER_SIZE 63

#define MSG_TYPE_DEBUG 2
#define MSG_TYPE_CALIBRATE 4
#define MSG_TYPE_STATUS 10
#define MSG_TYPE_ARM 11
#define MSG_TYPE_STARTUP 12
#define MSG_TYPE_SHUTDOWN 13
#define MSG_TYPE_MOTORS 14
#define MSG_TYPE_HEIGHT 16
#define MSG_TYPE_PID 18
#define MSG_TYPE_ERROR 0xff

#define MSG_METADATA_SIZE sizeof(uint8_t) + sizeof(uint8_t)
#define MAX_MSG_LEN BUFFER_SIZE - MSG_METADATA_SIZE

struct msg_struct{
	uint8_t type;
	uint8_t len;
	char msg[MAX_MSG_LEN];
};

class serial_comm{
public:
	void init(HardwareSerial* port);

	void zero_send();
	void zero_recv();

	void empty_recv();

	int append_msg(char* buff, uint32_t len);

	int send();
	int recv();

	HardwareSerial *port;

	struct msg_struct send_msg;
	struct msg_struct recv_msg;

};
