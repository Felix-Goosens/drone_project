import socket
import select
import struct

class drone_configuration:
	def __init__(self):
		self.max_motor_val = 0
		self.min_motor_val = 0

		self.height_P = 0
		self.height_I = 0
		self.height_D = 0

		self.yaw_P = 0
		self.yaw_I = 0
		self.yaw_D = 0

		self.roll_P = 0
		self.roll_I = 0
		self.roll_D = 0

		self.pitch_P = 0
		self.pitch_I = 0
		self.pitch_D = 0

class drone_status:
	def __init__(self):
		self.type = 0
		self.len = 0
		self.pres = 0
		self.temp = 0
		self.alt = 0

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

		self.send_msg(b"",self.MSG_TYPE_ARM)

	def send_msg(self,msg,msg_type):
		if(len(msg) > 61):
			print("Error: message is too long!")
			return
		self.sock.sendto(bytes([msg_type]) + bytes([len(msg)]) + msg,(self.drone_ip,self.drone_port))

	def send_debug_msg(self,msg):
		self.send_msg(msg,self.MSG_TYPE_DEBUG)

	def send_calibrate_msg(self,magnetic_declination):
		self.send_msg(struct.pack("f",magnetic_declination),self.MSG_TYPE_CALIBRATE)
		read_sockets, write_sockets, error_sockets = select.select([self.sock] , [], [], 60)
		for s in read_sockets:
			data = s.recv(4096)
			print(data)

	def send_status_msg(self):
		self.send_msg(b"",self.MSG_TYPE_STATUS)

	def send_startup_msg(self,configuration):
		msg = struct.pack("d",configuration.max_motor_val)
		msg += struct.pack("d",configuration.min_motor_val)

		msg += struct.pack("d",configuration.height_P)
		msg += struct.pack("d",configuration.height_I)
		msg += struct.pack("d",configuration.height_D)

		msg += struct.pack("d",configuration.yaw_P)
		msg += struct.pack("d",configuration.yaw_I)
		msg += struct.pack("d",configuration.yaw_D)

		msg += struct.pack("d",configuration.roll_P)
		msg += struct.pack("d",configuration.roll_I)
		msg += struct.pack("d",configuration.roll_D)

		msg += struct.pack("d",configuration.pitch_P)
		msg += struct.pack("d",configuration.pitch_I)
		msg += struct.pack("d",configuration.pitch_D)

		self.send_msg(msg,self.MSG_TYPE_STARTUP)

	def send_shutdown_msg(self):
		self.send_msg(b"",self.MSG_TYPE_SHUTDOWN)

	def send_motor_msg(self,m1,m2,m3,m4):
		if(m1 >= 1 << 16 or m2 >= 1 << 16 or m3 >= 1 << 16 or m4 >= 1 << 16):
			print("Error: one of the motor values is too large!")
			return
		m1 = int.to_bytes(m1,length=2,byteorder='little')
		m2 = int.to_bytes(m2,length=2,byteorder='little')
		m3 = int.to_bytes(m3,length=2,byteorder='little')
		m4 = int.to_bytes(m4,length=2,byteorder='little')

		self.send_msg(m1+m2+m3+m4,self.MSG_TYPE_MOTORS)

	def send_height_msg(self,target_height):
		self.send_msg(struct.pack("d",target_height),self.MSG_TYPE_HEIGHT)

	def recv_status(self,ds):
		try:
			read_sockets, write_sockets, error_sockets = select.select([self.sock] , [], [], 1)
		except Exception:
			return -1
		for s in read_sockets:
			data = s.recv(4096)
			if(data):
				ds.type = data[0]
				ds.len = data[1]
				ds.temp = struct.unpack("f",data[2:6])[0]
				ds.pres = struct.unpack("f",data[6:10])[0]
				ds.alt = struct.unpack("f",data[10:14])[0]

				ds.pitch = struct.unpack("f",data[14:18])[0]
				ds.roll = struct.unpack("f",data[18:22])[0]
				ds.yaw = struct.unpack("f",data[22:26])[0]
		return 0

if __name__ == "__main__":
	import argparse

	parser = argparse.ArgumentParser(prog='Drone Command', description='Send a command to the drone')
	parser.add_argument("command",action="store",help="The command for the drone")
	parser.add_argument("-c","--contents",action="store",help="Contents of the message")
	args = parser.parse_args()

	dc = drone_comm()
	dc.send_msg(args.contents.encode(),int(args.command))
