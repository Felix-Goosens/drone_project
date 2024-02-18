import curses
import math
import subprocess
import time

class interface:
    def __init__(self,stdscr):
        self.stdscr = stdscr
        curses.curs_set(False)
        curses.cbreak()
        stdscr.nodelay(True)

        self.option = 0
        self.last_input = time.time()
        self.pitch_degree = 0
        self.roll_degree = 0
        self.yaw_degree = 0
        self.height = 0
        self.delay = 0

    # https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#Similar_algorithms
    def line(self,pos_0,pos_1,symbol):
        x0 = pos_0[0]
        y0 = pos_0[1]

        x1 = pos_1[0]
        y1 = pos_1[1]

        if x0 == x1:
            for y in range(y0, y1+1, 1):
                self.stdscr.addstr(y, x0, symbol)
            for y in range(y1, y0+1, 1):
                self.stdscr.addstr(y, x0, symbol)
            return
        elif y0 == y1:
            for x in range(x0, x1+1, 1):
                self.stdscr.addstr(y0, x, symbol)
            for x in range(x1, x0+1, 1):
                self.stdscr.addstr(y0, x, symbol)
            return

        if abs(y1 - y0) < abs(x1 - x0):
            if x0 > x1:
                x0 = pos_1[0]
                y0 = pos_1[1]

                x1 = pos_0[0]
                y1 = pos_0[1]

            dx = x1 - x0
            dy = y1 - y0
            yi = 1
            if dy < 0:
                yi = -1
                dy = -dy
            D = (2 * dy) - dx
            y = y0

            for x in range(x0, x1+1, 1):
                self.stdscr.addstr(y, x, symbol)
                if D > 0:
                    y = y + yi
                    D = D + (2 * (dy - dx))
                else:
                    D = D + 2*dy
        else:
            if y0 > y1:
                x0 = pos_1[0]
                y0 = pos_1[1]

                x1 = pos_0[0]
                y1 = pos_0[1]

            dx = x1 - x0
            dy = y1 - y0
            xi = 1
            if dx < 0:
                xi = -1
                dx = -dx
            D = (2 * dx) - dy
            x = x0

            for y in range(y0, y1+1, 1):
                self.stdscr.addstr(y, x, symbol)
                if D > 0:
                    x = x + xi
                    D = D + (2 * (dx - dy))
                else:
                    D = D + 2*dx

    def gauge(self,pos,radius,degree,text=""):
        if text != "":
            self.stdscr.addstr(round(pos[1] - radius - 2), pos[0] - len(text) // 2, text)
        x2 = round(math.sin(math.pi/180*degree)*radius + pos[0])
        y2 = round(math.cos(math.pi/180*degree)*radius + pos[1])
        self.line(pos,(x2,y2),"*")
    
    def double_gauge(self,pos,length,degrees,text=""):
        self.gauge(pos, length/2, degrees, text)
        self.gauge(pos, length/2, degrees+180)

    def bar(self,pos,length,val,text="",ticks=[]):
        if text != "":
            self.stdscr.addstr(round(pos[1] - length/2 - 2), pos[0] - len(text) // 2, text)
        x0 = pos[0]
        y0 = pos[1]
        for y in range(y0-round(length/2)+1, y0+round(length/2), 1):
            self.stdscr.addstr(y, x0-1, "|")
            self.stdscr.addstr(y, x0+1, "|")
            if ticks != []:
                tick = ticks[y-(y0-round(length/2)+1)]
                self.stdscr.addstr(y,x0+2,f"{tick}")
        
        y_val = round(length / 2 * -val + y0)
        self.stdscr.addstr(y_val, x0, "*")

        for y in range(y0, y_val, 1):
            self.stdscr.addstr(y, x0, "-")

        for y in range(y_val+1, y0+1, 1):
            self.stdscr.addstr(y, x0, "-")
    
    def single_bar(self,pos,length,val,text="",ticks=[]):
        if text != "":
            self.stdscr.addstr(round(pos[1] - 2), pos[0] - len(text) // 2, text)
        x0 = pos[0]
        y0 = pos[1]
        for y in range(y0, y0+length, 1):
            self.stdscr.addstr(y, x0-1, "|")
            self.stdscr.addstr(y, x0+1, "|")
            if ticks != []:
                tick = ticks[y-y0]
                self.stdscr.addstr(y,x0+2,f"{tick}")
        
        y_val = round(-val*length + y0 + length)
        self.stdscr.addstr(y_val, x0, "*")

        for y in range(y_val + 1, y0 + length, 1):
            self.stdscr.addstr(y, x0, "-")

    def menu(self,pos,options,selected_option,text=""):
        if text != "":
            self.stdscr.addstr(round(pos[1] - 2), pos[0] - 1, text)

        x0 = pos[0]
        y0 = pos[1]
        self.stdscr.addstr(y0,x0,"----------")
        for i,option in enumerate(options):
            if i == selected_option:
                self.stdscr.addstr(y0+1+i,x0-1,">")
            self.stdscr.addstr(y0+1+i,x0+1,option)
            self.stdscr.addstr(y0+1+i,x0+10,"|")

        self.stdscr.addstr(y0+1+len(options),x0,"----------")

    def loop(self):
        # Pitch
        self.double_gauge((12,12),20,self.pitch_degree,"Pitch")
        # Roll
        self.double_gauge((34,12),20,self.roll_degree,"Roll")

        # Yaw
        self.stdscr.addstr(7-5-1,56,"N")
        self.gauge((56,12),10,self.yaw_degree,"Yaw")

        # Height
        if self.height > 50:
            height_ratio = 1
        elif self.height < -50:
            height_ratio = -1
        else:
            height_ratio = self.height/50
        self.bar((70,13), 22, height_ratio, "Height", ticks=[" 50m", "", " 40m", "", " 30m", "", " 20m", "", " 10m", "", " 0m", "", "-10m", "", "-20m", "", "-30m", "", "-40m", "", "-50m"])

#        # Delay
        if self.delay > 250:
            delay_ratio = 1
        else:
            delay_ratio = self.delay/250
#        self.single_bar((50,2),13,delay_ratio,"Delay",ticks=[" 1s", "", "", " 750ms", "", "", " 500ms", "", "", " 250ms", "", "", " 0s"])

        # Command Menu
        self.menu((80,2),["Arm", "Halt", "Exit"], self.option, "Command Menu")


if __name__ == "__main__":
    def main(stdscr):
        ui = interface(stdscr)
        while(True):
            stdscr.clear()
            ui.loop()
            stdscr.refresh()
            time.sleep(1/60)
    curses.wrapper(main).split("\n")
