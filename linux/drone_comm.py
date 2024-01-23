import socket
import select
import struct

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
		self.MSG_TYPE_STATUS = 10
		self.MSG_TYPE_MOTORS = 14

	def send_msg(self,msg,msg_type):
		if(len(msg) > 61):
			print("Error: message is too long!")
			return
		self.sock.sendto(bytes([msg_type]) + bytes([len(msg)]) + msg,(self.drone_ip,self.drone_port))

	def send_debug_msg(self,msg):
		self.send_msg(msg,self.MSG_TYPE_DEBUG)

	def send_status_msg(self):
		self.send_msg(b"",self.MSG_TYPE_STATUS)

	def send_motor_msg(self,m1,m2,m3,m4):
		if(m1 >= 1 << 16 or m2 >= 1 << 16 or m3 >= 1 << 16 or m4 >= 1 << 16):
			print("Error: one of the motor values is too large!")
			return
		m1 = int.to_bytes(m1,length=2,byteorder='little')
		m2 = int.to_bytes(m2,length=2,byteorder='little')
		m3 = int.to_bytes(m3,length=2,byteorder='little')
		m4 = int.to_bytes(m4,length=2,byteorder='little')

		self.send_msg(m1+m2+m3+m4,self.MSG_TYPE_MOTORS)

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
				
				ds.ax = struct.unpack("h",data[14:16])[0]
				ds.ay = struct.unpack("h",data[16:18])[0]
				ds.az = struct.unpack("h",data[18:20])[0]

				ds.gx = struct.unpack("h",data[20:22])[0]
				ds.gy = struct.unpack("h",data[22:24])[0]
				ds.gz = struct.unpack("h",data[24:26])[0]

				ds.mx = struct.unpack("h",data[26:28])[0]
				ds.my = struct.unpack("h",data[28:30])[0]
				ds.mz = struct.unpack("h",data[30:32])[0]
		return 0




