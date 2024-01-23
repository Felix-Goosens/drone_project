from drone_comm import *
import curses
import time

def main(stdscr):
	curses.curs_set(False)
	dc = drone_comm()
	ds = drone_status()
	t = time.time()
	output = ""
	count = 0
	try:
		while(True):
			count += 1
			while(True):
				dc.send_status_msg()
				if(dc.recv_status(ds) == 0):
					break
			if(ds.len > 0):
				output += "#########################\n"
				output += f"Time: {str(time.time())}\n"
				output += f"Type: {str(ds.type)}\n"
				output += f"Len: {str(ds.len)}\n"

				output += f"Temp: {str(ds.temp)}\n"
				output += f"Pressure: {str(ds.pres)}\n"
				output += f"Altitude: {str(ds.alt)}\n"

				output += f"ax: {str(ds.ax)}\n"
				output += f"ay: {str(ds.ay)}\n"
				output += f"az: {str(ds.az)}\n"

				output += f"gx: {str(ds.gx)}\n"
				output += f"gy: {str(ds.gy)}\n"
				output += f"gz: {str(ds.gz)}\n"

				output += f"mx: {str(ds.mx)}\n"
				output += f"my: {str(ds.my)}\n"
				output += f"mz: {str(ds.mz)}\n"

				stdscr.clear()
				stdscr.addstr(0,0,"#########################")
				stdscr.addstr(1,0,f"Time: {str(time.time())}")
				stdscr.addstr(2,0,f"Type: {str(ds.type)}")
				stdscr.addstr(3,0,f"Len: {str(ds.len)}")

				stdscr.addstr(5,0,f"Temp: {str(ds.temp)}")
				stdscr.addstr(6,0,f"Pressure: {str(ds.pres)}")
				stdscr.addstr(7,0,f"Altitude: {str(ds.alt)}")

				stdscr.addstr(9,0,f"ax: {str(ds.ax)}")
				stdscr.addstr(10,0,f"ay: {str(ds.ay)}")
				stdscr.addstr(11,0,f"az: {str(ds.az)}")

				stdscr.addstr(13,0,f"gx: {str(ds.gx)}")
				stdscr.addstr(14,0,f"gy: {str(ds.gy)}")
				stdscr.addstr(15,0,f"gz: {str(ds.gz)}")

				stdscr.addstr(17,0,f"mx: {str(ds.mx)}")
				stdscr.addstr(18,0,f"my: {str(ds.my)}")
				stdscr.addstr(19,0,f"mz: {str(ds.mz)}")
				stdscr.refresh()

				ds.len = 0
			if count > 1000:
				with open("gyro_data","a") as f:
					f.write(output)
					output = ""
					count = 0
			time.sleep(0.020)
	except Exception:
		with open("gyro_data","a") as f:
			f.write(output)
			output = ""
			count = 0

#	if(time.time()-t >= 5):
#		t = time.time()
#		dc.send_debug_msg(b"Hello,world!")
#		dc.send_motor_msg(1100,1100,1100,1100)
#		time.sleep(3)
#		dc.send_motor_msg(0,0,0,0)

curses.wrapper(main)