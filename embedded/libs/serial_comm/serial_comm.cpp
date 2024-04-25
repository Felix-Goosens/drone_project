#include "serial_comm.h"

void serial_comm::init(HardwareSerial *port){
	this->port = port;
}

int serial_comm::append_msg(char* buff, uint32_t len){
	if(this->send_msg.len + len > MAX_MSG_LEN){
		Serial.println("Append failed");
		return -1;
	}
	Serial.println("Appending");
	memcpy(&this->send_msg.msg[this->send_msg.len], buff, len);
	this->send_msg.len += len;
	return 0;
}

bool serial_comm::send(uint8_t msg_type){
	if(this->port->availableForWrite() >= sizeof(this->start_seq) + MSG_METADATA_SIZE + this->send_msg.len){
		this->port->write(this->start_seq, sizeof(this->start_seq));

		this->send_msg.type = msg_type;
		this->send_msg.checksum = this->compute_checksum(&this->send_msg);
		this->send_msg.header_checksum = this->compute_header_checksum(&this->send_msg);

		this->port->write((char*)&this->send_msg, MSG_METADATA_SIZE+this->send_msg.len);
		this->send_msg.len = 0;
		return true;
	}
	return false;
}

uint8_t serial_comm::compute_header_checksum(struct msg_struct* msg){
	uint8_t checksum = msg->checksum + msg->len + msg->type;
	return checksum;
}

bool serial_comm::validate_header_checksum(struct msg_struct* msg){
	return msg->header_checksum == this->compute_header_checksum(msg);
}

uint8_t serial_comm::compute_checksum(struct msg_struct* msg){
	uint8_t checksum = 0;
	for(int i=0;i<msg->len;i++){
		checksum += msg->msg[i];
	}
	return checksum;
}

bool serial_comm::validate_checksum(struct msg_struct* msg){
	return msg->checksum == this->compute_checksum(msg);
}

int serial_comm::read_start_seq(){
	static uint8_t sequence_counter = 0;

	while(sequence_counter < sizeof(this->start_seq) && this->port->available()){
		uint8_t c = this->port->read();
		if(c == this->start_seq[sequence_counter]){
			sequence_counter += 1;
		}
		else{
			sequence_counter = 0;
		}
	}
	if(sequence_counter == sizeof(this->start_seq)){
		sequence_counter = 0;
		// start sequence encountered, go to next state
		return 1;
	}
	// no start sequence found, maintain state
	return 0;
}

int serial_comm::read_header(){
	if(this->port->available() >= MSG_METADATA_SIZE){
		this->port->readBytes((char*)&this->recv_msg, MSG_METADATA_SIZE);
		if(!this->validate_header_checksum(&this->recv_msg)){
			// checksum failed, go to error state
			return 4;
		}
		// message received, go to next state
		return 2;
	}
	// wait for message, maintain state
	return 1;
}

int serial_comm::read_msg(){
	if(this->port->available() >= this->recv_msg.len){
		this->port->readBytes((char*)&this->recv_msg.msg, this->recv_msg.len);
		if(!this->validate_checksum(&this->recv_msg)){
			// checksum failed, go to error state
			return 4;
		}
		// message received, go to next state
		return 3;
	}
	// wait for message, maintain state
	return 2;
}

bool serial_comm::recv(){
	static int state = 0;
	switch(state){
		case(0):
			state = this->read_start_seq();
			break;
		case(1):
			state = this->read_header();
			break;
		case(2):
			state = this->read_msg();
			break;
		case(3):
			state = 0;
			return true;
		case(4):
			state = 0;
			return false;
	}
	return false;
}