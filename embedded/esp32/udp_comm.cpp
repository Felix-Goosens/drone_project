#include "udp_comm.h"

void udp_comm::init(){
	WiFi.softAP("TP-www", "thereisnospoon");
	this->udp.begin(1123);
}

bool udp_comm::send(){
	this->udp.beginPacket("192.168.4.2", 1123);
	this->udp.write(((const uint8_t*)&this->send_msg) + 2, this->send_msg.len + UDP_MSG_METADATA_SIZE);
	this->udp.endPacket();

	return true;
}

int udp_comm::read_header(){
	if(this->udp.available() >= UDP_MSG_METADATA_SIZE){
		this->udp.read(((char*)&this->recv_msg) + 2, UDP_MSG_METADATA_SIZE);
		return 1;
	}
	return 0;
}

int udp_comm::read_msg(){
	if(this->udp.available() >= this->recv_msg.len){
		this->udp.read((char*)&this->recv_msg.msg, this->recv_msg.len);
		return 2;
	}
	return 1;
}

bool udp_comm::recv(){
	static int state = 0;

	this->udp.parsePacket();

	switch(state){
		case(0):
			state = this->read_header();
			break;
		case(1):
			state = this->read_msg();
			break;
		case(2):
			state = 0;
			return true;
	}

	return false;
}