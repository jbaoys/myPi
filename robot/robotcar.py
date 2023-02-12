#!/usr/bin/python
'''
L298N           RPi GPIO        RPi Pin

ENA             GPIO13 (PMW1)   33
IN1             GPIO5           29
IN2             GPIO6           31
IN3             GPIO14          8
IN4             GPIO15          10
ENB             GPIO18 (PWM0)   12

'''
#import tty
import sys
import termios
import curses
import RPi.GPIO as IO
from time import sleep

#orig_settings = termios.tcgetattr(sys.stdin)
#tty.setcbreak(sys.stdin)

IO.setwarnings(False)   #do not show any warnings
IO.setmode(IO.BOARD)    #we are programming the GPIO by BCM pin numbers. (PIN33 as 'GPIO13')
arrow = { 'A':"up", 'B':"down", 'C':"right", 'D':"left" }
arrow2 = { 258:"down", 259:"up", 260:"left", 261:"right" }

ENA = 33
IN1 = 29
IN2 = 31
IN3 = 8
IN4 = 10
ENB = 12

class myCar:
    def __init__(self, ena, in1, in2, in3, in4, enb, hz = 50):
        self.ena = ena
        self.in1 = in1
        self.in2 = in2
        self.in3 = in3
        self.in4 = in4
        self.enb = enb
        self.hz = hz

        IO.setup(self.ena,IO.OUT)    #initialize GPIO13 as an output.
        IO.setup(self.in1,IO.OUT)
        IO.setup(self.in2,IO.OUT)
        IO.setup(self.in3,IO.OUT)
        IO.setup(self.in4,IO.OUT)
        IO.setup(self.enb,IO.OUT)    #initialize GPIO18 as an output.

        IO.output(self.in1, False)   #Turn off all motors
        IO.output(self.in2, False)
        IO.output(self.in3, False)
        IO.output(self.in4, False)

        self.pwma = IO.PWM(self.ena, self.hz)   #set pin self.ena as PWM output, with self.hz frequency
        self.pwma.start(0)                      #generate PWM signal with 0% duty cycle
        self.pwmb = IO.PWM(self.enb, self.hz)   #set pin self.enb as PWM output, with self.hz frequency
        self.pwmb.start(0)                      #generate PWM signal with 0% duty cycle

        self.dir = {
            "stop" : 0b0000,
            "fwrdLeft" : 0b0001,
            "bwrdLeft" : 0b0010,
            "stop1" : 0b0011,
            "bwrdRight" : 0b0100,
            "counterclock" : 0b0101,
            "backward" : 0b0110,
            "bwrdRight1" : 0b0111,
            "fwrdRight" : 0b1000,
            "forward" : 0b1001,
            "clockwise" : 0b1010,
            "fwrdRight1" : 0b1011,
            "stop2" : 0b1100,
            "fwrdLeft1" : 0b1101,
            "bwrdLeft1" : 0b1110,
            "stop3" : 0b1111
        }

    def __setHz(self, pwmPin, speed = 100):
        x = speed/100.0 * self.hz + 10
        pwmPin.ChangeFrequency(x)

    def __setDirection(self, dir):
        dircode = self.dir[dir]
        dir1 = (dircode & 0b1000) != 0
        dir2 = (dircode & 0b0100) != 0
        dir3 = (dircode & 0b0010) != 0
        dir4 = (dircode & 0b0001) != 0
        IO.output(self.in1, dir1)
        IO.output(self.in2, dir2)
        IO.output(self.in3, dir3)
        IO.output(self.in4, dir4)

    def stop(self):
        self.pwma.stop()
        self.pwmb.stop()

    def forward(self, speed, duration):
        self.__setDirection("forward")
        self.__setHz(self.pwma)
        self.__setHz(self.pwmb)
        self.pwma.start(speed)
        self.pwmb.start(speed)
        #sleep(duration)
        #self.stop()

    def backward(self, speed, duration):
        self.__setDirection("backward")
        self.__setHz(self.pwma)
        self.__setHz(self.pwmb)
        self.pwma.start(speed)
        self.pwmb.start(speed)
        #sleep(duration)
        #self.stop()

    def clockwise(self, speed, duration):
        self.__setDirection("clockwise")
        self.__setHz(self.pwma)
        self.__setHz(self.pwmb)
        self.pwma.start(speed)
        self.pwmb.start(speed)
        #sleep(duration)
        #self.stop()

    def counterclock(self, speed, duration):
        self.__setDirection("counterclock")
        self.__setHz(self.pwma)
        self.__setHz(self.pwmb)
        self.pwma.start(speed)
        self.pwmb.start(speed)
        #sleep(duration)
        #self.stop()

    def fwrdLeft(self, speed, duration, cycleSize=0):
        speeda = speed * (cycleSize/100.0)
        speedb = speed
        print ("speeda=%f" % speeda)
        self.__setHz(self.pwma, cycleSize)
        self.__setHz(self.pwmb)
        if cycleSize != 0:
            self.__setDirection("forward")
        else:
            self.__setDirection("fwrdLeft")
        self.pwma.start(speeda)
        self.pwmb.start(speedb)
        #sleep(duration)
        #self.stop()

    def fwrdRight(self, speed, duration, cycleSize=0):
        speeda = speed
        speedb = speed * (cycleSize/100.0)
        self.__setHz(self.pwma)
        self.__setHz(self.pwmb, cycleSize)
        if cycleSize != 0:
            self.__setDirection("forward")
        else:
            self.__setDirection("fwrdRight")
        self.pwma.start(speeda)
        self.pwmb.start(speedb)
        #sleep(duration)
        #self.stop()

    def bwrdLeft(self, speed, duration, cycleSize=0):
        speeda = speed * (cycleSize/100.0)
        speedb = speed
        self.__setHz(self.pwma, cycleSize)
        self.__setHz(self.pwmb)
        if cycleSize != 0:
            self.__setDirection("backward")
        else:
            self.__setDirection("bwrdLeft")
        self.pwma.start(speeda)
        self.pwmb.start(speedb)
        #sleep(duration)
        #self.stop()

    def bwrdRight(self, speed, duration, cycleSize=0):
        speeda = speed
        speedb = speed * (cycleSize/100.0)
        self.__setHz(self.pwma)
        self.__setHz(self.pwmb, cycleSize)
        if cycleSize != 0:
            self.__setDirection("backward")
        else:
            self.__setDirection("bwrdRight")
        self.pwma.start(speeda)
        self.pwmb.start(speedb)
        #sleep(duration)
        #self.stop()


def driver(bot, dir, duration=0.1):
    pwr = 80
    if dir=="up":
        bot.forward(pwr, duration)
    elif dir=="down":
        bot.backward(pwr, duration)
    elif dir=="right":
        bot.clockwise(pwr, duration)
    elif dir=="left":
        bot.counterclock(pwr, duration)

robotCar = myCar(ENA, IN1, IN2, IN3, IN4, ENB)
def keyloop(stdscr):
    #sleepTime = int(stdscr.getstr())
    while (1):
        curses.flushinp()
        stdscr.clear()
        stdscr.addstr("Key in:")
        doit = stdscr.getch()
        stdscr.addstr("\n")
        stdscr.addstr("Input ")
        print("%u" % doit)
        stdscr.refresh()

        if doit == ord('N') or doit == ord('n'):
            robotCar.stop()
            break
        if doit >= 258 and doit <= 261:
            x = arrow2[doit]
            driver(robotCar, x)

        sleep(0.05)
        robotCar.stop()

if __name__=='__main__':
    curses.wrapper(keyloop)

'''
if __name__=='__main__':
    try:
        while True:
            x=sys.stdin.read(1)
            if x=='\x1b':
                x=sys.stdin.read(1)
                if x=='[':
                    x=sys.stdin.read(1)
                    x=arrow[x]
                    driver(robotCar, x)
            print(x)
    except KeyboardInterrupt:
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, orig_settings)  
        robotCar.stop()
    '''

'''
    robotCar.forward(100, 1)
    sleep(1)
    robotCar.backward(50, 1)
    sleep(1)
    robotCar.fwrdLeft(50, 1)
    sleep(1)
    robotCar.fwrdRight(50, 1)
    sleep(1)
    robotCar.fwrdLeft(50, 1, 50)
    sleep(1)
    robotCar.fwrdRight(50, 1, 50)
    sleep(1)
    robotCar.bwrdLeft(50, 1)
    sleep(1)
    robotCar.bwrdRight(50, 1)
    sleep(1)
    robotCar.bwrdLeft(50, 1, 50)
    sleep(1)
    robotCar.bwrdRight(50, 1, 50)
    sleep(1)

try:
    while 1:                #execute loop forever

        for x in notes:
            p.ChangeFrequency(x)
            time.sleep(0.1)

        for x in range (hz, hz+500):
            p.ChangeFrequency(x)
            time.sleep(0.1)
        for x in range (50):        #execute loop for 50 times, x being incremented from 0 to 49.
            p.ChangeDutyCycle(x)    #change duty cycle for varying the brightness of LED.
            time.sleep(0.1)         #sleep for 100m second

        for x in range (50):        #execute loop for 50 times, x being incremented from 0 to 49.
            p.ChangeDutyCycle(50-x) #change duty cycle for changing the brightness of LED.
            time.sleep(0.1)         #sleep for 100m second
except KeyboardInterrupt:
    p.stop()
    IO.cleanup()
'''
