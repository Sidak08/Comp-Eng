"""
This Raspberry Pi code was developed by newbiely.com
This Raspberry Pi code is made available for public use without any restriction
For comprehensive instructions and wiring diagrams, please visit:
https://newbiely.com/tutorials/raspberry-pi/raspberry-pi-led-fade
"""

import time

import RPi.GPIO as GPIO

LED_PIN = 18
FADE_PERIOD = 3000
FADE_IN_MODE = True
FADE_OUT_MODE = False

fadeMode = FADE_IN_MODE
start_time = 0
brightness = 0


def _map(x, in_min, in_max, out_min, out_max):
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)


GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_PIN, GPIO.OUT)
pwm = GPIO.PWM(LED_PIN, 1000)
pwm.start(0)
start_time = time.time() * 1000
try:
    while True:
        progress = time.time() * 1000 - start_time
        if fadeMode == FADE_IN_MODE:
            if progress <= FADE_PERIOD:
                brightness = _map(progress, 0, FADE_PERIOD, 0, 100)

                print(brightness)
                pwm.ChangeDutyCycle(brightness)
            else:
                start_time = time.time() * 1000
                fadeMode = FADE_OUT_MODE
                progress = 0
        if fadeMode == FADE_OUT_MODE:
            if progress <= FADE_PERIOD:
                brightness = 100 - _map(progress, 0, FADE_PERIOD, 0, 100)

                print(brightness)
                pwm.ChangeDutyCycle(brightness)
            else:
                start_time = time.time() * 1000
                fadeMode = FADE_IN_MODE
                progress = 0
finally:
    pwm.stop()
    GPIO.cleanup()
