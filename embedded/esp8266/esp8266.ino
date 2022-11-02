#include <serial_comm.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include "udp_comm.h"

serial_comm mega2560_comm(&Serial);
udp_comm udp;

void setup() {
	Serial.begin(9600);
//	pinMode(2, OUTPUT);
//	digitalWrite(2, HIGH);
//	delay(2000);
//	digitalWrite(2, LOW);
}

void loop() {
	if(mega2560_comm.recv() == 0){
		memcpy(&udp.send_msg,&mega2560_comm.recv_msg,MSG_METADATA_SIZE+mega2560_comm.recv_msg.len);
		udp.send();
	}
	if(udp.recv()){
		memcpy(&mega2560_comm.send_msg,&udp.recv_msg,MSG_METADATA_SIZE+udp.recv_msg.len);
		mega2560_comm.send();
	}
}
