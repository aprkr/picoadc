#!/usr/bin/python3

# Importing Libraries 
import matplotlib.pyplot as plt
import serial 
import time 
pico = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=0.5) 

plt.ion()
x = []
y = []
i = 0
BUF_SIZE = 81920
NUM_SECS = 1
SAMPLING_RATE = 200000

NUM_SAMPLES = SAMPLING_RATE * NUM_SECS
pico.write(0x1)
while True:
    i += BUF_SIZE
    value = list(pico.read(BUF_SIZE))
    # x = range(i)
    y = y + value
    print(i)
    if (i > NUM_SAMPLES):
        break
        
x= range(i)
plt.plot(x[0:len(y)],y)
# plt.plot(x[4000000:4000100],y[4000000:4000100])
plt.draw()

plt.show(block=True) # block=True lets the window stay open at the end of the animation