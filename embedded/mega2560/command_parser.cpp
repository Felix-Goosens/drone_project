#include "command_parser.h"

int command_parser::execute_command(struct msg_struct* command){
	uint16_t* motor_values = 0;
	float* magnetic_declination;
	int *value_range;
	struct pid_factors_struct PID_factors[4];

	switch(command->type){
#ifdef DEBUG
		case(CMD_TYPE_DEBUG):
			Serial.println(command->msg);

			ESP_COMM.send_msg.type = MSG_TYPE_DEBUG;
			break;
#endif
		case(CMD_TYPE_MOTORS):
#ifdef DEBUG
			Serial.println("CMD_TYPE_MOTORS");
#endif
			if(command->len != sizeof(uint16_t)*4){
				ESP_COMM.send_msg.type = MSG_TYPE_ERROR;
				break;
			}
			motor_values = (uint16_t*)command->msg;
#ifdef DEBUG
			Serial.println("Changing motor settings:");
			Serial.print("Moter 1: ");
			Serial.println(motor_values[0],DEC);
			Serial.print("Moter 2: ");
			Serial.println(motor_values[1],DEC);
			Serial.print("Moter 3: ");
			Serial.println(motor_values[2],DEC);
			Serial.print("Moter 4: ");
			Serial.println(motor_values[3],DEC);
#endif
			M1.speed(motor_values[0]);
			M2.speed(motor_values[1]);
			M3.speed(motor_values[2]);
			M4.speed(motor_values[3]);

			ESP_COMM.send_msg.type = MSG_TYPE_MOTORS;
			break;
		case(CMD_TYPE_STATUS):
#ifdef DEBUG
			Serial.println("CMD_TYPE_STATUS");
#endif
			ESP_COMM.append_msg((char*)&MPU_DEV.pitch,sizeof(MPU_DEV.pitch));
			ESP_COMM.append_msg((char*)&MPU_DEV.roll,sizeof(MPU_DEV.roll));
			ESP_COMM.append_msg((char*)&MPU_DEV.yaw,sizeof(MPU_DEV.yaw));

			ESP_COMM.append_msg((char*)&FC.height_correction,sizeof(FC.height_correction));
			ESP_COMM.append_msg((char*)&FC.yaw_correction,sizeof(FC.yaw_correction));
			ESP_COMM.append_msg((char*)&FC.roll_correction,sizeof(FC.roll_correction));
			ESP_COMM.append_msg((char*)&FC.pitch_correction,sizeof(FC.pitch_correction));

			ESP_COMM.append_msg((char*)&FC.target_height,sizeof(FC.target_height));
			ESP_COMM.append_msg((char*)&FC.height,sizeof(FC.height));

			ESP_COMM.send_msg.type = MSG_TYPE_STATUS;
			break;
		case(CMD_TYPE_CAL):
#ifdef DEBUG
			Serial.println("CMD_TYPE_CAL");
#endif
			if(command->len != sizeof(float)){
				ESP_COMM.send_msg.type = MSG_TYPE_ERROR;
				break;
			}
			magnetic_declination = (float*)command->msg;
			MPU_DEV.calibrate(*magnetic_declination);

			ESP_COMM.send_msg.type = MSG_TYPE_CALIBRATE;
			break;
		case(CMD_TYPE_ARM):
#ifdef DEBUG
			Serial.println("CMD_TYPE_ARM");
#endif
			M1.arm();
			M2.arm();
			M3.arm();
			M4.arm();

			ESP_COMM.send_msg.type = MSG_TYPE_ARM;
			break;
		case(CMD_TYPE_STARTUP):
#ifdef DEBUG
			Serial.println("CMD_TYPE_STARTUP");
#endif
			if(command->len != sizeof(int)*2 + sizeof(struct pid_factors_struct)*2){
				ESP_COMM.send_msg.type = MSG_TYPE_ERROR;
				break;
			}
			value_range = (int*)command->msg;
			PID_factors[0] = ((struct pid_factors_struct *)((int*)command->msg+2))[0];
			PID_factors[1] = ((struct pid_factors_struct *)((int*)command->msg+2))[1];

			ESP_COMM.send_msg.type = MSG_TYPE_STARTUP;
			while(ESP_COMM.send() != 0){}

			while(ESP_COMM.recv() != 0){}
			if(command->type != CMD_TYPE_STARTUP || command->len != sizeof(struct pid_factors_struct)*2){
				ESP_COMM.send_msg.type = MSG_TYPE_ERROR;
				break;
			}
			PID_factors[2] = ((struct pid_factors_struct *)command->msg)[0];
			PID_factors[3] = ((struct pid_factors_struct *)command->msg)[1];

			FC.init(PID_factors[0],PID_factors[1],PID_factors[2],PID_factors[3],value_range[0],value_range[1]);
#ifdef DEBUG
			Serial.println("Starting up FC:");
/*			Serial.print("Min val: ");
			Serial.println(FC.min_motor_val,DEC);
			Serial.print("Max val: ");
			Serial.println(FC.max_motor_val,DEC);

			Serial.print("Height PID 1: P=");
			Serial.print(FC.height_pid.P_factor,DEC);
			Serial.print(",I=");
			Serial.print(FC.height_pid.I_factor,DEC);
			Serial.print(",D=");
			Serial.println(FC.height_pid.D_factor,DEC);

			Serial.print("Yaw PID 1: P=");
			Serial.print(FC.yaw_pid.P_factor,DEC);
			Serial.print(",I=");
			Serial.print(FC.yaw_pid.I_factor,DEC);
			Serial.print(",D=");
			Serial.println(FC.yaw_pid.D_factor,DEC);

			Serial.print("Roll PID 1: P=");
			Serial.print(FC.roll_pid.P_factor,DEC);
			Serial.print(",I=");
			Serial.print(FC.roll_pid.I_factor,DEC);
			Serial.print(",D=");
			Serial.println(FC.roll_pid.D_factor,DEC);

			Serial.print("Pitch PID 1: P=");
			Serial.print(FC.pitch_pid.P_factor,DEC);
			Serial.print(",I=");
			Serial.print(FC.pitch_pid.I_factor,DEC);
			Serial.print(",D=");
			Serial.println(FC.pitch_pid.D_factor,DEC);*/
#endif
			STARTUP = true;

			ESP_COMM.send_msg.type = MSG_TYPE_STARTUP;
			break;
		case(CMD_TYPE_SHUTDOWN):
#ifdef DEBUG
			Serial.println("CMD_TYPE_SHUTDOWN");
#endif
			STARTUP = false;

			ESP_COMM.send_msg.type = MSG_TYPE_SHUTDOWN;
			break;
		case(CMD_TYPE_HEIGHT):
#ifdef DEBUG
			Serial.println("CMD_TYPE_HEIGHT");
#endif
			if(command->len != sizeof(float)){
				ESP_COMM.send_msg.type = MSG_TYPE_ERROR;
				break;
			}
			FC.target_height += *(float*)(command->msg);
			
			ESP_COMM.send_msg.type = MSG_TYPE_HEIGHT;
			break;
		default:
#ifdef DEBUG
			Serial.print("Unknown message type: ");
			Serial.println(command->type,DEC);
#endif
			ESP_COMM.send_msg.type = MSG_TYPE_ERROR;
			break;
	}
	ESP_COMM.empty_recv();
	while(ESP_COMM.send() != 0){}
	return 0;
}