from drone_comm import *
import curses
import time

def main(stdscr):
	curses.curs_set(False)
	dc = drone_comm()
	ds = drone_status()
	t = time.time()
	while(True):
		dc.recv_status(ds)
		if(ds.len > 0):
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
#	if(time.time()-t >= 5):
#		t = time.time()
#		dc.send_debug_msg(b"Hello,world!")
#		dc.send_motor_msg(1100,1100,1100,1100)
#		time.sleep(3)
#		dc.send_motor_msg(0,0,0,0)

curses.wrapper(main)