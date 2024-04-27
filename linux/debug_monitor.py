import time
import random

import matplotlib.pyplot
import matplotlib.animation as animation
import numpy

import drone_comm

class generator:
    def __init__(self):
        self.var = random.random()
        self.var -= random.random()

    def gen(self):
        self.var += random.random()
        self.var -= random.random()
        return b"var1: " + str(self.var).encode() + b",var2: " + str(self.var*time.time()**0.1).encode()

class analyzer:
    def __init__(self, source, f):
        self.source = source
        self.filter = f
        self.lines = {}

    def collect(self):
        start = time.time()
        while True:
            t = time.time() - start
            yield t, self.source()

    def draw(self, data):
        t = data[0]
        data = data[1]
        for prop_name in data:
            prop_val = data[prop_name]

            ax = matplotlib.pyplot.gca()
            line = self.lines[prop_name]
            x = list(line.get_xdata())
            x.append(t)

            xmin, xmax = ax.get_xlim()
            if xmax <= t:
                ax.set_xlim(xmin, xmax + 5)
                ax.figure.canvas.draw()

            y = list(line.get_ydata())
            y.append(prop_val)

            ymin, ymax = ax.get_ylim()
            if prop_val >= ymax:
                ax.set_ylim(ymin, 1.2*ymax)
                ax.figure.canvas.draw()
            elif prop_val <= ymin:
                ax.set_ylim(1.2*ymin, ymax)
                ax.figure.canvas.draw()

            line.set_data(x, y)

        return tuple(self.lines.values())

    def run(self):
        matplotlib.pyplot.ion()
        fig, self.ax = matplotlib.pyplot.subplots()
#        self.ax.set_ylim(-1, 1)
#        self.ax.set_xlim(0, 5)
#        print(self.source().keys())
#        exit()
#        self.lines = {f : self.ax.plot([], [], lw=2, label=f)[0] for f in self.filter}
        self.lines = {k : self.ax.plot([], [], lw=2, label=k)[0] for k in self.source().keys()}

#        ani = animation.FuncAnimation(fig=fig, func=self.draw, frames=self.collect, blit=True, save_count=100,interval=100)
        ani = animation.FuncAnimation(fig=fig, func=self.draw, frames=self.collect, blit=False, save_count=100, interval=100)
        matplotlib.pyplot.legend()
        matplotlib.pyplot.show()
        while True:
            fig.canvas.start_event_loop(0.001)
#            print("TEST")

if __name__ == "__main__":
    dc = drone_comm.drone_comm()

    a = analyzer(dc.send_debug_msg, None)
    a.run()