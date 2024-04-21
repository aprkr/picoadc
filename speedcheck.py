#!/usr/bin/python3

# Importing Libraries 
import matplotlib.pyplot as plt
import serial 
import time 
pico = serial.Serial(port='/dev/ttyACM1', baudrate=115200, timeout=0.5) 

plt.ion()
x = []
y = []
i = 0
pico.write(0x1)
while True:
    i += 2048
    value = list(pico.read(2048))
    x = range(i)
    y = y + value
    if (i > 100000):
        break
        

# plt.plot(x[99900:100000], y[99900:100000])
plt.plot(x,y)
plt.draw()

plt.show(block=True) # block=True lets the window stay open at the end of the animation