#!/usr/bin/env python
import time
'''
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BOARD)

GPIO.setup(12, GPIO.OUT)

p = GPIO.PWM(12, 50)

p.start(0)
delay = 3

try:
    while True:
        p.ChangeDutyCycle(5)  # turn left towards -90 degree
        p.stop()
        time.sleep(delay) # sleep 1 second
        p.ChangeDutyCycle(7.5)  # turn neutral towards 0 degree
        time.sleep(delay) # sleep 1 second
        p.ChangeDutyCycle(10) # turn right towards +90 degree
        time.sleep(delay) # sleep 1 second 
except KeyboardInterrupt:
    p.stop()
    GPIO.cleanup()

'''

'''
.=======================================================================.
| refer to http://abyz.me.uk/rpi/pigpio/python.html                     |
| This module uses the services of the C pigpio library. pigpio must be |
| running on the Pi(s) whose GPIO are to be manipulated. The normal way |
| to start pigpio is as a daemon (during system start).                 |
|                                                                       |
| This test uses SG90 servo. It has 3 pins, red is connected to 5v,     |
| brown is connected to GND, orange is connected to GPIO18              |
|                                                                       |
| pi@raspberrypi:~ $ sudo pigpiod                                       |
`======================================================================='
'''
import pigpio

pwmPin = 18
pi = pigpio.pi()    # Connect to local Pi's GPIO.
delay = 2
try:
    while True:
        pi.set_servo_pulsewidth(pwmPin, 500)    # Clockwise to the end (180 degree)
        time.sleep(delay)
        pi.set_servo_pulsewidth(pwmPin, 1500)   # The middle (90 degree)
        time.sleep(delay)
        pi.set_servo_pulsewidth(pwmPin, 2500)   # Counter-clockewise to the end (0 degree)
        time.sleep(delay)

except KeyboardInterrupt:
    pi.set_servo_pulsewidth(pwmPin,0)
    pi.stop()

