#pragma once

#include <serial_comm.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

class udp_comm : serial_comm {
public:
	udp_comm();

	WiFiUDP udp;

	void empty_recv();

	using serial_comm::zero_send;
	using serial_comm::zero_recv;

	using serial_comm::append_msg;

	int send();
	int recv();

	using serial_comm::send_msg;
	using serial_comm::recv_msg;
};
