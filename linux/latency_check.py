from drone_comm import *
import numpy
import math
import time
dc = drone_comm()
ds = drone_status()
timeout = 60
latencies = []

dc.recv_status(ds)
start_time = time.time()
last_recv_time = start_time

while(time.time() - start_time < timeout):
	dc.recv_status(ds)
	if(ds.len > 0):
		latencies.append(time.time() - last_recv_time)
		last_recv_time = time.time()
		ds.len = 0

latencies.sort()
print("### Latencies Report ###")
print(f"Timeout: {timeout}")
print(f"Number of Messages: {len(latencies)}")
print(f"Average Latency: {numpy.average(latencies)}")
print(f"Average Latency (Top 5%): {numpy.average(latencies[math.floor(len(latencies)*0.95):])}")
print(f"Average Latency (Bottom 5%): {numpy.average(latencies[:math.ceil(len(latencies)*0.05)])}")
