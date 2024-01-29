from drone_comm import *
import argparse
import curses
import time
import numpy
import matplotlib.pyplot as plt

error_log = ""
parser = argparse.ArgumentParser(prog='measure', description='Measure the sensor values of the drone')
parser.add_argument("-c","--calibrate",action="store_true",help="Calibrate the MPU before measurement")
parser.add_argument("-m","--motors",action="store_true",help="Turn on the motors during measurment")
parser.add_argument("-a","--analyze",action="store",help="Analyze an output file instead of making new measurements")
parser.add_argument("-o","--output",action="store",help="Place the output in a file")
parser.add_argument("-t","--time",action="store",default=10,help="Time in seconds to measure")
parser.add_argument("-n","--names",action="append",help="Names of the units to analyze")
args = parser.parse_args()

def main(stdscr):
    global error_log
    curses.curs_set(False)

    dc = drone_comm()
    ds = drone_status()
    start_time = time.time()
    count = 0
    measurements = ""

    if args.calibrate:
        try:
            dc.send_calibrate_msg(2.16)
        except:
            error_log += "Error: calibrating drone\n"
            return

    if args.motors:
        dc.send_motor_msg(1100,1100,1100,1100)

    while(True):
        while(True):
            dc.send_status_msg()
            if(dc.recv_status(ds) == 0):
                break
        if(ds.len > 0):
            count += 1
            measurements_per_second = count / (time.time() - start_time)
            stdscr.clear()
            stdscr.addstr(0,0,"#########################")
            stdscr.addstr(1,0,f"Time: {str(time.time()-start_time)}")
            stdscr.addstr(2,0,f"Measurements / second: {str(measurements_per_second)}")
            stdscr.addstr(4,0,f"roll: {str(ds.roll)}")
            stdscr.addstr(5,0,f"pitch: {str(ds.pitch)}")
            stdscr.addstr(6,0,f"yaw: {str(ds.yaw)}")
            stdscr.addstr(7,0,f"Pressure: {str(ds.pres)}")
            stdscr.addstr(8,0,f"Temp: {str(ds.temp)}")

            stdscr.refresh()

            measurements += f"#########################\n"
            measurements += f"Time: {str(time.time()-start_time)}\n"
            measurements += f"roll: {str(ds.roll)}\n"
            measurements += f"pitch: {str(ds.pitch)}\n"
            measurements += f"yaw: {str(ds.yaw)}\n"
            measurements += f"Pressure: {str(ds.pres)}\n"
            measurements += f"Temperature: {str(ds.temp)}\n"

        if time.time() - start_time > args.time:
            break

    if args.output != None:
        with open(args.output,"w") as f:
            f.write(measurements)
    
    if args.motors:
        dc.send_motor_msg(0,0,0,0)
    return measurements

def analyze_data(measurements,names):
    x = []
    y = {n:[] for n in names}
    for line in measurements:
        if "Time" in line:
            x.append(float(line[5:]))
        elif line.split(":")[0] in names:
            y[line.split(":")[0]].append(float(line.split(":")[1]))
    y = {n:numpy.array(y[n]) for n in names}

    fig, ax = plt.subplots(1,1)

    for name in names:
        print(name)
        print(f"Average: {y[name].mean()}")
        print(f"Std: {y[name].std()}")
#        points[1] = (points[1]-101325)/12.013
#        ax[0].scatter(x, y[name], s=1)
        ax.scatter(x, y[name], s=1)

    plt.show()


if args.analyze != None:
    with open(args.analyze,"r") as f:
        measurements = f.readlines()
else:
    measurements = curses.wrapper(main).split("\n")

print(error_log,end="")
if error_log != "":
    exit

analyze_data(measurements,args.names)