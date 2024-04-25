#pragma once

#include <serial_comm.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#define UDP_MSG_METADATA_SIZE sizeof(uint8_t) * 2

class udp_comm : serial_comm {
public:
	void init();

	WiFiUDP udp;

	void empty_recv();

	using serial_comm::append_msg;

	int read_header();
	int read_msg();

	bool send();
	bool recv();

	using serial_comm::send_msg;
	using serial_comm::recv_msg;
};