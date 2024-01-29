import matplotlib.pyplot as plt
import numpy as np

def get_datapoints(name):
    dump = open("roll_data_moving","r")
    x = []
    y = []
    for line in dump:
        if "Time" in line:
            x.append(float(line[5:]))
        if name in line:
            try:
                y.append(int(line[len(name)+1:]))
            except:
                y.append(float(line[len(name)+1:]))
    x = [t-x[0] for t in x]
    dump.close()
    x = np.array(x)
    if type(y[0]) != type(1):
        y = np.array(y)
    else:
        y = np.array(y,dtype=np.int16)
    return [x,y]

def offset(y):
    mean = y.mean()
    y = np.array([(p - mean) for p in y],dtype=type(y[0]))
    return y

def average(y,n=0):
    if n == 0:
        n = len(y)

    avg = 0
    count = 0
    while(len(y) - count >= n):
        for i in range(n):
            avg += y[count+i]
        avg /= n
        for i in range(n):
            y[count+i] = avg

        count += n
    
    return y

def plot_points(names,real=False):
    fig, ax = plt.subplots(1,3)
    if not real:
        ax[0].set_ylim([-2**16/2-1,2**16/2-1])
        ax[1].set_ylim([-2**16/2-1,2**16/2-1])
        ax[2].set_ylim([-2**16/2-1,2**16/2-1])

    for name in names:
        points = get_datapoints(name)
        print(name)
        print(f"Average: {points[1].mean()}")
        print(f"Std: {points[1].std()}")
        points[1] = (points[1]-101325)/12.013
        ax[0].scatter(points[0], points[1], s=1)
        points[1] = offset(points[1])

        ax[1].scatter(points[0], points[1], s=1)
        points[1] = average(points[1],n=5)

        ax[2].scatter(points[0], points[1], s=1)
    
    plt.show()

#plot_points(["roll","pitch","yaw"],True)
#plot_points(["mx","my","mz"])
#plot_points(["ax","ay","az"])
#plot_points(["gx","gy","gz"])
plot_points(["Pressure"],True)
