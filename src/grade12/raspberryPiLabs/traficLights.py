"""
This Raspberry Pi code was developed by newbiely.com
This Raspberry Pi code is made available for public use without any restriction
For comprehensive instructions and wiring diagrams, please visit:
https://newbiely.com/tutorials/raspberry-pi/raspberry-pi-traffic-light
"""

import time

import RPi.GPIO as GPIO

# Define GPIO pins
PIN_RED = (
    7  # The Raspberry Pi GPIO pin connected to the R pin of the traffic light module
)
PIN_YELLOW = (
    8  # The Raspberry Pi GPIO pin connected to the Y pin of the traffic light module
)
PIN_GREEN = (
    25  # The Raspberry Pi GPIO pin connected to the G pin of the traffic light module
)

# Define time durations in seconds
RED_TIME = 2  # RED time in seconds
YELLOW_TIME = 1  # YELLOW time in seconds
GREEN_TIME = 2  # GREEN time in seconds

# Define indices for the light states
RED = 0
YELLOW = 1
GREEN = 2

# Create lists for pins and times
pins = [PIN_RED, PIN_YELLOW, PIN_GREEN]
times = [RED_TIME, YELLOW_TIME, GREEN_TIME]

# Set up GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(PIN_RED, GPIO.OUT)
GPIO.setup(PIN_YELLOW, GPIO.OUT)
GPIO.setup(PIN_GREEN, GPIO.OUT)

light = RED  # start with RED light
last_time = time.time()


def traffic_light_on(light):
    for i in range(len(pins)):
        if i == light:
            GPIO.output(pins[i], GPIO.HIGH)  # turn on
        else:
            GPIO.output(pins[i], GPIO.LOW)  # turn off


try:
    while True:
        if (time.time() - last_time) > times[light]:
            light += 1
            if light >= 3:
                light = RED  # new circle

            traffic_light_on(light)
            last_time = time.time()

        # TO DO: your other code

except KeyboardInterrupt:
    # Clean up GPIO on exit
    GPIO.cleanup()
s
