"""
This Raspberry Pi code was developed by newbiely.com
This Raspberry Pi code is made available for public use without any restriction
For comprehensive instructions and wiring diagrams, please visit:
https://newbiely.com/tutorials/raspberry-pi/raspberry-pi-light-sensor-led
"""

import time

import RPi.GPIO as GPIO

# Set the GPIO mode to BCM
GPIO.setmode(GPIO.BCM)

# Define the PIN numbers for the light sensor and the LED
LIGHT_SENSOR_PIN = 15
LED_PIN = 12

# Set up the light sensor PIN as an input with a pull-up resistor
GPIO.setup(LIGHT_SENSOR_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Set up the LED PIN as an output
GPIO.setup(LED_PIN, GPIO.OUT)

try:
    while True:
        # Read the state of the light sensor
        light_state = GPIO.input(LIGHT_SENSOR_PIN)

        # Control the LED based on the light sensor state
        if light_state == GPIO.LOW:
            # Light is present, turn off the LED
            GPIO.output(LED_PIN, GPIO.LOW)
        else:
            # Light is not present, turn on the LED
            GPIO.output(LED_PIN, GPIO.HIGH)

        # Small delay to avoid rapid state changes
        time.sleep(1)

except KeyboardInterrupt:
    # Clean up GPIO settings on program exit
    GPIO.cleanup()
