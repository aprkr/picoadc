#!/usr/bin/python3

# Importing Libraries 
import matplotlib.pyplot as plt
import serial 
import time 
# import numpy
from array import *
pico = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=2.0) 

plt.ion()
x = []
y = array('H',[])
array = array('H',[])
i = 0
BUF_SIZE = 112000
NUM_SECS = 5
SAMPLING_RATE = 200000

NUM_SAMPLES = SAMPLING_RATE * NUM_SECS
pico.write(0x1)
while True:
    i += 56000
    value = pico.read(BUF_SIZE)
    array.frombytes(value)
    x = range(i)
    print(i)
    if (i > NUM_SAMPLES):
        break
        
x= range(i)
y= array
plt.plot(x[0:len(y)],y)
# plt.plot(x[0:100],y[0:100])
plt.draw()

pico.close()
plt.show(block=True) # block=True lets the window stay open at the end of the animation