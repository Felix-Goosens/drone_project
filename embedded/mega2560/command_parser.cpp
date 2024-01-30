#include "command_parser.h"

int command_parser::execute_command(struct msg_struct* command){
	uint16_t* motor_values = 0;

	switch(command->type){
#ifdef DEBUG
		case(CMD_TYPE_DEBUG):
			Serial.println(command->msg);
			break;
#endif
		case(CMD_TYPE_MOTORS):
#ifdef DEBUG
			Serial.println("CMD_TYPE_MOTORS");
#endif
			if(command->len != sizeof(uint16_t)*4){
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

			break;
		case(CMD_TYPE_STATUS):
#ifdef DEBUG
			Serial.println("CMD_TYPE_STATUS");
#endif
			ESP_COMM.send_msg.type = MSG_TYPE_STATUS;

			ESP_COMM.append_msg((char*)&MPU_DEV.temp,sizeof(MPU_DEV.temp));
			ESP_COMM.append_msg((char*)&MPU_DEV.pressure,sizeof(MPU_DEV.pressure));
			ESP_COMM.append_msg((char*)&MPU_DEV.altitude,sizeof(MPU_DEV.altitude));

			ESP_COMM.append_msg((char*)&MPU_DEV.pitch,sizeof(MPU_DEV.pitch));
			ESP_COMM.append_msg((char*)&MPU_DEV.roll,sizeof(MPU_DEV.roll));
			ESP_COMM.append_msg((char*)&MPU_DEV.yaw,sizeof(MPU_DEV.yaw));

			ESP_COMM.send();
			break;
		case(CMD_TYPE_CAL):
#ifdef DEBUG
			Serial.println("CMD_TYPE_CAL");
#endif
			if(command->len != sizeof(float)){
				break;
			}
			float* magnetic_declination = (float*)command->msg;
			MPU_DEV.calibrate(*magnetic_declination);

			ESP_COMM.send_msg.type = MSG_TYPE_CALIBRATE;
			ESP_COMM.send();
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
			ESP_COMM.send();
			break;
		case(CMD_TYPE_STARTUP):
#ifdef DEBUG
			Serial.println("CMD_TYPE_STARTUP");
#endif
			if(command->len != sizeof(int)*2 + sizeof(struct pid_factors_struct)*4){
				break;
			}
			int *value_range = (int*)command->msg;
			struct pid_factors_struct *PID_factors = (struct pid_factors_struct *)((int*)command->msg+2);

			FC.init(PID_factors[0],PID_factors[1],PID_factors[2],PID_factors[3],value_range[0],value_range[1]);
#ifdef DEBUG
			Serial.println("Starting up FC:");
			Serial.print("Min val: ");
			Serial.println(FC->min_motor_val,DEC);
			Serial.print("Max val: ");
			Serial.println(FC->max_motor_val,DEC);

			Serial.print("Height PID 1: P=");
			Serial.print(FC->height_pid.P_factor,DEC);
			Serial.print(",I=");
			Serial.print(FC->height_pid.I_factor,DEC);
			Serial.print(",D=");
			Serial.println(FC->height_pid.D_factor,DEC);

			Serial.print("Yaw PID 1: P=");
			Serial.print(FC->yaw_pid.P_factor,DEC);
			Serial.print(",I=");
			Serial.print(FC->yaw_pid.I_factor,DEC);
			Serial.print(",D=");
			Serial.println(FC->yaw_pid.D_factor,DEC);

			Serial.print("Roll PID 1: P=");
			Serial.print(FC->roll_pid.P_factor,DEC);
			Serial.print(",I=");
			Serial.print(FC->roll_pid.I_factor,DEC);
			Serial.print(",D=");
			Serial.println(FC->roll_pid.D_factor,DEC);

			Serial.print("Pitch PID 1: P=");
			Serial.print(FC->pitch_pid.P_factor,DEC);
			Serial.print(",I=");
			Serial.print(FC->pitch_pid.I_factor,DEC);
			Serial.print(",D=");
			Serial.println(FC->pitch_pid.D_factor,DEC);
#endif
			ESP_COMM.send_msg.type = MSG_TYPE_STARTUP;
			ESP_COMM.send();
			STARTUP = true;
			break;
		case(CMD_TYPE_SHUTDOWN):
			STARTUP = false;
			ESP_COMM.send_msg.type = MSG_TYPE_SHUTDOWN;
			ESP_COMM.send();
			break;
		case(CMD_TYPE_HEIGHT):
#ifdef DEBUG
			Serial.println("CMD_TYPE_HEIGHT");
#endif
			if(command->len != sizeof(double)){
				break;
			}

			FC.target_height = *(double*)(command->msg);
			ESP_COMM.send_msg.type = MSG_TYPE_HEIGHT;
			ESP_COMM.send();
			break;
		default:
#ifdef DEBUG
			Serial.print("Unknown message type: ");
			Serial.println(command->type,DEC);
#endif
			break;
	}
	return 0;
}