#include "udp_comm.h"

void udp_comm::init(){
	WiFi.softAP("TP-www", "thereisnospoon");
	this->udp.begin(1123);
	this->empty_recv();
}

void udp_comm::empty_recv(){
	while(this->udp.parsePacket() > 0){}
}

int udp_comm::send(){
	this->udp.beginPacket("192.168.4.2", 1123);
	this->udp.write((const uint8_t*)&this->send_msg, MSG_METADATA_SIZE + this->send_msg.len);
	this->udp.endPacket();
	this->zero_send();
	return 1;
}

int udp_comm::recv(){
	int size = this->udp.parsePacket();
	if(size > 0){
		this->zero_recv();
		if(size > sizeof(struct msg_struct) || size < MSG_METADATA_SIZE){
			return 0;
		}
		this->udp.read((char*)&this->recv_msg, size);
		if(!this->recv_msg.len > MAX_MSG_LEN){
			this->empty_recv();
			this->zero_recv();
			return 0;
		}
		return 1;
	}
	return 0;
}