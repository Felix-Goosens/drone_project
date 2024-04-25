#include <serial_comm.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <device_status.h>
#include <timing.h>
#include "udp_comm.h"

const int LED_PIN = 33;

serial_comm mega2560_comm;
udp_comm udp;

device_status_class dev_status(LED_PIN,-1,LED_PIN);
timing_class timing;
size_t heartbeat_t;

void setup(){
	Serial.begin(9600);
	udp.init();
	mega2560_comm.init(&Serial);
	heartbeat_t = timing.add_timing(1000);
}

void loop(){
	if(timing.is_time(heartbeat_t)){
		dev_status.heartbeat();
	}
	if(mega2560_comm.recv()){
		memcpy(&udp.send_msg, &mega2560_comm.recv_msg, MSG_METADATA_SIZE + mega2560_comm.recv_msg.len);
		udp.send();
	}
	if(udp.recv()){
		// Send an ack to indicate that the message was received
		udp.send_msg.type = udp.recv_msg.type;
		udp.send_msg.len = 0;
		udp.send();

		memcpy(&mega2560_comm.send_msg.msg, &udp.recv_msg.msg, udp.recv_msg.len);
		mega2560_comm.send_msg.len = udp.recv_msg.len;
		while(!mega2560_comm.send(udp.recv_msg.type)){}
	}
}