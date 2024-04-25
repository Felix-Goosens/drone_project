#pragma once
#include <Arduino.h>

#define BUFFER_SIZE SERIAL_TX_BUFFER_SIZE

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

#define MSG_METADATA_SIZE sizeof(uint8_t) * 4
#define MAX_MSG_LEN 255

struct msg_struct{
	uint8_t header_checksum;
	uint8_t checksum;
	uint8_t type;
	uint8_t len;
	char msg[MAX_MSG_LEN];
};

class serial_comm{
	uint8_t start_seq[4] = {0xf0, 0x0f, 0x10, 0x01};
public:
	void init(HardwareSerial* port);

	int append_msg(char* buff, uint32_t len);

	uint8_t compute_header_checksum(struct msg_struct* msg);
	bool validate_header_checksum(struct msg_struct* msg);

	uint8_t compute_checksum(struct msg_struct* msg);
	bool validate_checksum(struct msg_struct* msg);

	int read_start_seq();
	int read_header();
	int read_msg();

	bool send(uint8_t msg_type);
	bool recv();

	HardwareSerial *port;

	struct msg_struct send_msg;
	struct msg_struct recv_msg;

};
