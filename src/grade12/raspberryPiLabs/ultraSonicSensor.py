"""
This Raspberry Pi code was developed by newbiely.com
This Raspberry Pi code is made available for public use without any restriction
For comprehensive instructions and wiring diagrams, please visit:
https://newbiely.com/tutorials/raspberry-pi/raspberry-pi-ultrasonic-sensor-piezo-buzzer

Modified for continuous distance monitoring with improved error handling
"""

import signal
import sys
import time

import RPi.GPIO as GPIO

# GPIO pin numbers for the ultrasonic sensor
TRIG_PIN = 14
ECHO_PIN = 15

# GPIO pin number for the piezo buzzer
BUZZER_PIN = 16

# Threshold distance in centimeters
THRESHOLD_DISTANCE_CM = 7

# Delay between distance measurements (seconds)
MEASUREMENT_DELAY = 0.1

# Timeout for echo measurement (seconds)
ECHO_TIMEOUT = 0.1

# Constants for note names and their corresponding frequencies
C4 = 261
D4 = 293
E4 = 329
F4 = 349
G4 = 392
A4 = 440
B4 = 493

# Dictionary to map numeric values to note names
note_names = {
    C4: "C4",
    D4: "D4",
    E4: "E4",
    F4: "F4",
    G4: "G4",
    A4: "A4",
    B4: "B4",
}

# List of notes in the "Jingle Bells" melody
melody = [
    E4,
    E4,
    E4,
    E4,
    E4,
    E4,
    E4,
    G4,
    C4,
    D4,
    E4,
    F4,
    F4,
    F4,
    F4,
    F4,
    E4,
    E4,
    E4,
    E4,
    E4,
    D4,
    D4,
    E4,
    D4,
    G4,
]

# List of note durations (in milliseconds)
note_durations = [
    200,
    200,
    400,
    200,
    200,
    400,
    200,
    200,
    200,
    200,
    200,
    200,
    200,
    400,
    200,
    200,
    200,
    200,
    200,
    200,
    200,
    200,
    200,
    400,
    200,
    200,
]

# Pause duration between notes (in milliseconds)
pause_duration = 300


def signal_handler(signal, frame):
    """Handle Ctrl+C gracefully"""
    print("\nShutting down...")
    GPIO.cleanup()
    sys.exit(0)


def play_tone(pin, frequency, duration):
    """Play a tone on the specified pin with given frequency and duration"""
    # Calculate the period based on the frequency
    period = 1.0 / frequency

    # Calculate the time for half of the period
    half_period = period / 2.0

    # Calculate the number of cycles for the given duration
    cycles = int(duration / period)

    for _ in range(cycles):
        # Set the GPIO pin to HIGH
        GPIO.output(pin, GPIO.HIGH)

        # Wait for half of the period
        time.sleep(half_period)

        # Set the GPIO pin to LOW
        GPIO.output(pin, GPIO.LOW)

        # Wait for the other half of the period
        time.sleep(half_period)


def play_jingle_bells():
    """Play the Jingle Bells melody"""
    print("Playing Jingle Bells melody...")
    for i in range(len(melody)):
        note_duration = note_durations[i] / 1000.0
        note_freq = melody[i]
        note_name = note_names.get(note_freq, "Pause")

        print(
            f"Playing {note_name} (Frequency: {note_freq} Hz) for {note_duration} seconds"
        )

        play_tone(BUZZER_PIN, note_freq, note_duration)

        time.sleep(pause_duration / 1000.0)

        GPIO.output(BUZZER_PIN, GPIO.LOW)


def get_distance():
    """Get distance measurement from ultrasonic sensor with timeout"""
    try:
        # Send trigger pulse
        GPIO.output(TRIG_PIN, GPIO.HIGH)
        time.sleep(0.00001)
        GPIO.output(TRIG_PIN, GPIO.LOW)

        # Wait for echo start with timeout
        timeout_start = time.time()
        while GPIO.input(ECHO_PIN) == 0:
            pulse_start = time.time()
            if pulse_start - timeout_start > ECHO_TIMEOUT:
                print("Warning: Echo timeout - no pulse start detected")
                return None

        # Wait for echo end with timeout
        timeout_start = time.time()
        while GPIO.input(ECHO_PIN) == 1:
            pulse_end = time.time()
            if pulse_end - timeout_start > ECHO_TIMEOUT:
                print("Warning: Echo timeout - pulse too long")
                return None

        # Calculate distance
        pulse_duration = pulse_end - pulse_start
        speed_of_sound = 34300  # Speed of sound in cm/s
        distance = (pulse_duration * speed_of_sound) / 2

        # Sanity check for reasonable distance values
        if distance > 400:  # Max range for most HC-SR04 sensors
            print("Warning: Distance reading seems unrealistic")
            return None

        return distance

    except Exception as e:
        print(f"Error measuring distance: {e}")
        return None


def setup_gpio():
    """Initialize GPIO settings"""
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(TRIG_PIN, GPIO.OUT)
    GPIO.setup(ECHO_PIN, GPIO.IN)
    GPIO.setup(BUZZER_PIN, GPIO.OUT)

    # Set initial states
    GPIO.output(TRIG_PIN, GPIO.LOW)
    GPIO.output(BUZZER_PIN, GPIO.LOW)

    # Allow sensor to settle
    time.sleep(2)
    print("GPIO setup complete. Sensor ready.")


def main():
    """Main program loop"""
    print("Starting continuous ultrasonic distance monitoring...")
    print(f"Threshold distance: {THRESHOLD_DISTANCE_CM} cm")
    print("Press Ctrl+C to exit")

    # Setup signal handler for graceful shutdown
    signal.signal(signal.SIGINT, signal_handler)

    # Initialize GPIO
    setup_gpio()

    last_melody_time = 0
    melody_cooldown = 5  # Minimum seconds between melody plays

    try:
        while True:
            distance = get_distance()

            if distance is not None:
                print(f"Distance: {distance:.2f} cm", end="")

                if distance < THRESHOLD_DISTANCE_CM:
                    print(" - OBJECT DETECTED!")

                    # Check if enough time has passed since last melody
                    current_time = time.time()
                    if current_time - last_melody_time > melody_cooldown:
                        play_jingle_bells()
                        last_melody_time = current_time
                    else:
                        # Just beep instead of full melody to avoid spam
                        print("Playing short beep...")
                        play_tone(BUZZER_PIN, A4, 0.1)
                        GPIO.output(BUZZER_PIN, GPIO.LOW)
                else:
                    print(" - Clear")
            else:
                print("Distance: ERROR - Could not get reading")

            # Small delay between measurements
            time.sleep(MEASUREMENT_DELAY)

    except Exception as e:
        print(f"Unexpected error: {e}")
    finally:
        print("\nCleaning up GPIO...")
        GPIO.cleanup()


if __name__ == "__main__":
    main()
