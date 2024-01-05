from drone_comm import *
import time
dc = drone_comm()
ds = drone_status()
t = time.time()
while(True):
	dc.recv_status(ds)
	if(ds.len > 0):
		print("#########################")
		print("Time: " + str(time.time()))

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
	if(time.time()-t >= 5):
		t = time.time()
		dc.send_debug_msg(b"Hello,world!")
		dc.send_motor_msg(1100,1100,1100,1100)
		time.sleep(3)
		dc.send_motor_msg(0,0,0,0)
