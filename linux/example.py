from drone_comm import *
import time
dc = drone_comm()
ds = drone_status()
t = time.time()
while(True):
	dc.recv_status(ds)
	if(ds.len > 0):
		print("Type: " + str(ds.type))
		print("Len: " + str(ds.len))
		print("Temp: " + str(ds.temp))
		print("Pressure: " + str(ds.pres))
		print("Altitude: " + str(ds.alt))

		print("ax: " + str(ds.ax))
		print("ay: " + str(ds.ay))
		print("az: " + str(ds.az))

		print("gx: " + str(ds.gx))
		print("gy: " + str(ds.gy))
		print("gz: " + str(ds.gz))

		print("mx: " + str(ds.mx))
		print("my: " + str(ds.my))
		print("mz: " + str(ds.mz))

		ds.len = 0
	if(time.time()-t >= 2):
		t = time.time()
		dc.send_debug_msg(b"Hello,world!")
		dc.send_motor_msg(1300,1300,333,444)
#		dc.send_motor_msg(0,1200,333,444)
		time.sleep(15)
		dc.send_motor_msg(0,0,0,0)
		exit()
