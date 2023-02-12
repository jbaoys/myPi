#!/usr/bin/python
'''
Use command "pinout" to display the GPIO pins:
J8:
   3V3  (1) (2)  5V    
 GPIO2  (3) (4)  5V    
 GPIO3  (5) (6)  GND   
 GPIO4  (7) (8)  GPIO14
   GND  (9) (10) GPIO15
GPIO17 (11) (12) GPIO18
GPIO27 (13) (14) GND   
GPIO22 (15) (16) GPIO23
   3V3 (17) (18) GPIO24
GPIO10 (19) (20) GND   
 GPIO9 (21) (22) GPIO25
GPIO11 (23) (24) GPIO8 
   GND (25) (26) GPIO7 
 GPIO0 (27) (28) GPIO1 
 GPIO5 (29) (30) GND   
 GPIO6 (31) (32) GPIO12
GPIO13 (33) (34) GND   
GPIO19 (35) (36) GPIO16
GPIO26 (37) (38) GPIO20
   GND (39) (40) GPIO21 (*)

'''
import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BCM)

#set GPIO Pins
GPIO_ECHO = 21

#set GPIO direction (IN/OUT)
GPIO.setup(GPIO_ECHO, GPIO.IN)

def my_callback(channel):
    if not my_callback.hit:
        print("HIT !!!")
        my_callback.hit = True

my_callback.hit = False

GPIO.add_event_detect(GPIO_ECHO, GPIO.FALLING, callback=my_callback)

if __name__ == '__main__':
    try:
        while True:
            print("start to shoot...")
            my_callback.hit = False
            while not my_callback.hit:
                time.sleep(0.001)
            time.sleep(1)
    except KeyboardInterrupt:
        print ("Measurement stopped by user")
        GPIO.cleanup()
