#pragma once
#include <Arduino.h>

#define BUFFER_SIZE 63

#define MSG_TYPE_DEBUG 2
#define MSG_TYPE_STATUS 10
#define MSG_TYPE_MOTORS 14
#define MSG_METADATA_SIZE sizeof(char) + sizeof(uint8_t)
#define MAX_MSG_LEN BUFFER_SIZE - MSG_METADATA_SIZE

struct msg_struct{
	char type;
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
