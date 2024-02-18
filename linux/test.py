from interface import interface
import log_measurement
import curses
from drone_comm import *
import time

dc = drone_comm()
ds = drone_status()
drone_conf = drone_configuration()
dc.send_startup_msg(drone_conf)
start_time = time.time()

def main(stdscr):
    ui = interface(stdscr)
    lm = log_measurement.log_measurement("interface_log")
    option = 0
    last_input = time.time()

    msg_delay = 0.250
    msg_time = 0

    while(True):
        key = stdscr.getch()
        dt = time.time() - last_input
        if key == curses.KEY_DOWN and dt > 0.250:
            option = (option + 1) % 3
            last_input = time.time()
        elif key == curses.KEY_UP and dt > 0.250:
            option = (option - 1) % 3
            last_input = time.time()
        elif key == curses.KEY_ENTER:
            if option == 0:
                configuration = drone_configuration()
                dc.send_startup_msg(configuration)
            elif option == 1:
                dc.send_shutdown_msg()
        elif key == ord("x"):
            dc.send_height_msg(-1)
        elif key == ord("z"):
            dc.send_height_msg(1)

        if(msg_delay < (time.time() - msg_time) and dc.send_status_msg(ds) == 0):
            lm.log(ds)
            ui.roll_degree = ds.roll + 180
            ui.pitch_degree = ds.pitch + 180
            ui.yaw_degree = ds.yaw + 180
            ui.height = ds.height
            ui.delay = (time.time() - msg_time) * 1000
            msg_time = time.time()

        ui.option = option

        stdscr.clear()
        ui.loop()
        stdscr.refresh()
        time.sleep(1/60)

curses.wrapper(main).split("\n")
