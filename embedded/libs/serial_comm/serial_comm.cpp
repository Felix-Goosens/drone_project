#include "serial_comm.h"

void serial_comm::init(HardwareSerial *port){
	this->port = port;
	this->zero_send();
	this->zero_recv();
	this->empty_recv();
}

void serial_comm::zero_send(){
	memset((void*)&this->send_msg,0,sizeof(this->send_msg));
}

void serial_comm::zero_recv(){
	memset((void*)&this->recv_msg,0,sizeof(this->recv_msg));
}

void serial_comm::empty_recv(){
	char tmp;
	while(this->port->available() > 0){
		this->port->readBytes(&tmp,1);
	}
}

int serial_comm::append_msg(char* buff, uint32_t len){
	if(this->send_msg.len + len > MAX_MSG_LEN){
		return -1;
	}
	memcpy(this->send_msg.msg + this->send_msg.len,buff,len);
	this->send_msg.len += len;
	return 0;
}

int serial_comm::send(){
	if(this->port->availableForWrite() >= MSG_METADATA_SIZE+this->send_msg.len){
		this->port->write((char*)&this->send_msg,MSG_METADATA_SIZE+this->send_msg.len);
		this->zero_send();
		return 0;
	}
	return -1;
}

int serial_comm::recv(){
	if(this->port->available() >= MSG_METADATA_SIZE){
		this->zero_recv();
		this->port->readBytes((char*)&this->recv_msg,MSG_METADATA_SIZE);
		if(recv_msg.len > MAX_MSG_LEN){
			this->empty_recv();
			this->zero_recv();
			return -1;
		}
		while(this->port->available() < this->recv_msg.len){}
		this->port->readBytes((char*)this->recv_msg.msg,this->recv_msg.len);
		return 0;
	}
	return -1;
}