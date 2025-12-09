"""
This Raspberry Pi code was developed by newbiely.com
This Raspberry Pi code is made available for public use without any restriction
For comprehensive instructions and wiring diagrams, please visit:
https://newbiely.com/tutorials/raspberry-pi/raspberry-pi-motion-sensor-servo-motor
"""

import time

import RPi.GPIO as GPIO

# Constants won't change
MOTION_SENSOR_PIN = (
    14  # Raspberry Pi GPIO pin connected to the OUTPUT pin of the motion sensor
)
SERVO_PIN = 12  # Raspberry Pi GPIO pin connected to the servo motor

# Variables will change
angle = 0  # The current angle of the servo motor
prev_motion_state = None  # The previous state of the motion sensor
motion_state = None  # The current state of the motion sensor

# Setup GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(MOTION_SENSOR_PIN, GPIO.IN)
GPIO.setup(SERVO_PIN, GPIO.OUT)

# Create PWM instance for servo
servo_pwm = GPIO.PWM(SERVO_PIN, 50)  # 50 Hz frequency

# Initialize servo position
servo_pwm.start(0)

try:
    while True:
        prev_motion_state = motion_state  # Save the last state
        motion_state = GPIO.input(MOTION_SENSOR_PIN)  # Read new state

        if motion_state == GPIO.LOW and prev_motion_state == GPIO.HIGH:
            print("Motion detected!")
            servo_pwm.ChangeDutyCycle(7.5)  # Adjust duty cycle for 90 degrees

        elif motion_state == GPIO.HIGH and prev_motion_state == GPIO.LOW:
            print("Motion stopped!")
            servo_pwm.ChangeDutyCycle(2.5)  # Adjust duty cycle for 0 degrees

        time.sleep(0.1)  # Add a small delay to avoid rapid sensor readings

except KeyboardInterrupt:
    servo_pwm.stop()
    GPIO.cleanup()
