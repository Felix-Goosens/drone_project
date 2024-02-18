import socket
import select
import struct

class drone_configuration:
	def __init__(self):
		self.max_motor_val = 1800
		self.min_motor_val = 1100

		self.height_P = 5
		self.height_I = 0.2
		self.height_D = 0

		self.yaw_P = 2
		self.yaw_I = 0
		self.yaw_D = 0

		self.roll_P = 0.6
		self.roll_I = 0
		self.roll_D = 0

		self.pitch_P = 0.6
		self.pitch_I = 0
		self.pitch_D = 0

class drone_status:
	def __init__(self):
		self.height_correction = 0
		self.yaw_correction = 0
		self.roll_correction = 0
		self.pitch_correction = 0

		self.target_height = 0
		self.height = 0

class drone_comm:
	def __init__(self):
		self.ip = "192.168.4.2"
		self.drone_ip = "192.168.4.1"
		self.port = 1123
		self.drone_port = 1123
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.sock.settimeout(1)
		self.sock.bind((self.ip, self.port))

		self.MSG_TYPE_DEBUG = 2
		self.MSG_TYPE_CALIBRATE = 4
		self.MSG_TYPE_STATUS = 10
		self.MSG_TYPE_ARM = 11
		self.MSG_TYPE_STARTUP = 12
		self.MSG_TYPE_SHUTDOWN = 13
		self.MSG_TYPE_MOTORS = 14
		self.MSG_TYPE_HEIGHT = 16

		if self.send_arm_msg() < 0:
			print("Error Arming Drone")
			exit()

	def recv_msg(self, timeout=1):
		try:
			read_sockets, write_sockets, error_sockets = select.select([self.sock] , [], [], timeout)
		except Exception:
			return -1,b""
		return 0,read_sockets[0].recv(4096)

	def send_msg(self,msg,msg_type):
		if(len(msg) > 61):
			print("Error: message is too long!")
			return
		self.sock.sendto(bytes([msg_type]) + bytes([len(msg)]) + msg,(self.drone_ip,self.drone_port))

		if msg_type == self.MSG_TYPE_CALIBRATE:
			s,ack = self.recv_msg(60)
		else:
			s,ack = self.recv_msg()

		if s == -1:
			print("Error: MSG Timeout")
		elif ack[0] != msg_type:
			print("Error: Received Unexpected Response")

		return s,ack

	def send_arm_msg(self):
		s,ack = self.send_msg(b"",self.MSG_TYPE_ARM)
		return s

	def send_debug_msg(self,msg):
		s,ack = self.send_msg(msg,self.MSG_TYPE_DEBUG)
		return s

	def send_calibrate_msg(self,magnetic_declination):
		s,ack = self.send_msg(struct.pack("f",magnetic_declination),self.MSG_TYPE_CALIBRATE)
		print(ack)
		return s

	def send_status_msg(self,ds):
		s,data = self.send_msg(b"",self.MSG_TYPE_STATUS)
		if s != 0:
			return s
		ds.pitch = struct.unpack("f",data[2:6])[0]
		ds.roll = struct.unpack("f",data[6:10])[0]
		ds.yaw = struct.unpack("f",data[10:14])[0]

		ds.height_correction = struct.unpack("f",data[14:18])[0]
		ds.yaw_correction = struct.unpack("f",data[18:22])[0]
		ds.roll_correction = struct.unpack("f",data[22:26])[0]
		ds.pitch_correction = struct.unpack("f",data[26:30])[0]

		ds.target_height = struct.unpack("f",data[30:34])[0]
		ds.height = struct.unpack("f",data[34:38])[0]
		return s

	def send_startup_msg(self,configuration):
		msg = struct.pack("h",configuration.max_motor_val)
		msg += struct.pack("h",configuration.min_motor_val)

		msg += struct.pack("f",configuration.height_P)
		msg += struct.pack("f",configuration.height_I)
		msg += struct.pack("f",configuration.height_D)

		msg += struct.pack("f",configuration.yaw_P)
		msg += struct.pack("f",configuration.yaw_I)
		msg += struct.pack("f",configuration.yaw_D)

		s,ack = self.send_msg(msg,self.MSG_TYPE_STARTUP)
		if s != 0:
			return s

		msg = struct.pack("f",configuration.roll_P)
		msg += struct.pack("f",configuration.roll_I)
		msg += struct.pack("f",configuration.roll_D)

		msg += struct.pack("f",configuration.pitch_P)
		msg += struct.pack("f",configuration.pitch_I)
		msg += struct.pack("f",configuration.pitch_D)

		s,ack = self.send_msg(msg,self.MSG_TYPE_STARTUP)
		return s

	def send_shutdown_msg(self):
		s,ack = self.send_msg(b"",self.MSG_TYPE_SHUTDOWN)
		return s

	def send_motor_msg(self,m1,m2,m3,m4):
		if(m1 >= 1 << 16 or m2 >= 1 << 16 or m3 >= 1 << 16 or m4 >= 1 << 16):
			print("Error: one of the motor values is too large!")
			return
		m1 = int.to_bytes(m1,length=2,byteorder='little')
		m2 = int.to_bytes(m2,length=2,byteorder='little')
		m3 = int.to_bytes(m3,length=2,byteorder='little')
		m4 = int.to_bytes(m4,length=2,byteorder='little')

		s,ack = self.send_msg(m1+m2+m3+m4,self.MSG_TYPE_MOTORS)
		return s

	def send_height_msg(self,target_height):
		s,ack = self.send_msg(struct.pack("f",target_height),self.MSG_TYPE_HEIGHT)
		return s

if __name__ == "__main__":
	import argparse

	parser = argparse.ArgumentParser(prog='Drone Command', description='Send a command to the drone')
	parser.add_argument("command",action="store",help="The command for the drone")
	parser.add_argument("-c","--contents",action="store",help="Contents of the message",default="")
	args = parser.parse_args()

	dc = drone_comm()
	if args.command == "MOTORS":
		speeds = args.contents.split(",")
		dc.send_motor_msg(int(speeds[0]),int(speeds[1]),int(speeds[2]),int(speeds[3]))
	elif args.command == "STATUS":
		ds = drone_status()
		dc.send_status_msg(ds)
		print(vars(ds))
	elif args.command == "CALIBRATE":
		dc.send_calibrate_msg(float(args.contents))
