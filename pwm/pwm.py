#!/usr/bin/python
'''
Using GPIO19, PIN35 as PWM output, connecting to a passive buzzer
'''
import RPi.GPIO as IO   #calling header file which helps us use GPIO's of PI
import time             #calling time to provide delays in program

# Music Note
A4=440
Ab4=466
B4=494
C5=523
Db5=554
D5=587
Eb5=622
E5=659
F5=698
Gb5=740
G5=784
Ab5=831

notes = [ A4, Ab4, B4, C5, Db5, D5, Eb5, E5, F5, Gb5, G5, Ab5,
          G5, Gb5, F5, E5, Eb5, D5, Db5, C5, B4, Ab4, A4 ]

IO.setwarnings(False)   #do not show any warnings

IO.setmode (IO.BCM)     #we are programming the GPIO by BCM pin numbers. (PIN35 as 'GPIO19')

IO.setup(19,IO.OUT)     # initialize GPIO19 as an output.

hz = 500
p = IO.PWM(19,hz)      #GPIO19 as PWM output, with 100Hz frequency
p.start(50)              #generate PWM signal with 0% duty cycle
try:
    while 1:                #execute loop forever

        for x in notes:
            p.ChangeFrequency(x)
            time.sleep(0.1)

        '''
        for x in range (hz, hz+500):
            p.ChangeFrequency(x)
            time.sleep(0.1)
        for x in range (50):        #execute loop for 50 times, x being incremented from 0 to 49.
            p.ChangeDutyCycle(x)    #change duty cycle for varying the brightness of LED.
            time.sleep(0.1)         #sleep for 100m second

        for x in range (50):        #execute loop for 50 times, x being incremented from 0 to 49.
            p.ChangeDutyCycle(50-x) #change duty cycle for changing the brightness of LED.
            time.sleep(0.1)         #sleep for 100m second
        '''
except KeyboardInterrupt:
    p.stop()
    IO.cleanup()
