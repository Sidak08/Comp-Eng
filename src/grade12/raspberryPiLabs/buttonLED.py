"""
This Raspberry Pi code was developed by newbiely.com
This Raspberry Pi code is made available for public use without any restriction
For comprehensive instructions and wiring diagrams, please visit:
https://newbiely.com/tutorials/raspberry-pi/raspberry-pi-button-led
"""

import time

import RPi.GPIO as GPIO

# Constants won't change
BUTTON_PIN = 16  # The number of the pushbutton pin
LED_PIN = 18  # The number of the LED pin

# Variables will change
led_state = GPIO.LOW  # The current state of the LED
prev_button_state = GPIO.LOW  # The previous state of the button
button_state = GPIO.LOW  # The current state of the button

# Set up GPIO
GPIO.setmode(GPIO.BCM)  # Use BCM GPIO numbering
GPIO.setup(LED_PIN, GPIO.OUT)  # Initialize the LED pin as an output
GPIO.setup(
    BUTTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP
)  # Initialize the pushbutton pin as a pull-up input

try:
    while True:
        # Read the state of the pushbutton value
        prev_button_state = button_state  # Save the last state
        button_state = GPIO.input(BUTTON_PIN)  # Read new state

        if prev_button_state == GPIO.HIGH and button_state == GPIO.LOW:
            time.sleep(0.1)  # 100 milliseconds debounce time
            print("The button is pressed")

            # Toggle the state of the LED
            if led_state == GPIO.LOW:
                led_state = GPIO.HIGH
            else:
                led_state = GPIO.LOW

            # Control LED according to the toggled state
            GPIO.output(LED_PIN, led_state)

except KeyboardInterrupt:
    # Clean up GPIO on program exit
    GPIO.cleanup()
