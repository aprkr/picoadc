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
BUF_SIZE = 81920
NUM_SECS = 2
SAMPLING_RATE = 80000

NUM_SAMPLES = SAMPLING_RATE * NUM_SECS
pico.write(0x1)
while True:
    i += 40960
    value = pico.read(BUF_SIZE)
    print(value)
    array.frombytes(value)
    print(array)
    # value = array.frombytes(value)
    # value = numpy.array(value, numpy.uint8)
    # value = value.view('<H')
    # print(value)
    x = range(i)
    # y = y + array
    print(i)
    if (i > NUM_SAMPLES):
        break
        
x= range(i)
y= array
plt.plot(x[0:len(y)],y)
# plt.plot(x[len(y)-100:len(y)],y[len(y)-100:len(y)])
plt.draw()

plt.show(block=True) # block=True lets the window stay open at the end of the animation