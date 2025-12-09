"""
This Raspberry Pi code was developed by newbiely.com
This Raspberry Pi code is made available for public use without any restriction
For comprehensive instructions and wiring diagrams, please visit:
https://newbiely.com/tutorials/raspberry-pi/raspberry-pi-keypad-servo-motor

Modified for continuous operation with proper cleanup and servo control.
"""

import time

import RPi.GPIO as GPIO

# Define keypad layout
KEYPAD = [[1, 2, 3], [4, 5, 6], [7, 8, 9], ["*", 0, "#"]]

# Define GPIO pins for rows, columns, and servo motor
ROW_PINS = [17, 27, 22, 24]
COL_PINS = [25, 8, 7]

SERVO_PIN = 20  # Adjust this to the actual GPIO pin connected to the servo motor

# Passwords to unlock
PASSWORDS = [
    [1, 2, 3, 4],  # 1st password 1234
    [5, 6, 7, 8],  # 2nd password 5678
    [9, 0, 1, 2],  # 3rd password 9012
]

# Maximum password length
MAX_PASSWORD_LENGTH = 6


def setup_gpio():
    """Initialize GPIO settings"""
    # Initialize GPIO
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)  # Disable warnings for continuous operation

    # Set up row pins as inputs with pull-up resistors
    for row_pin in ROW_PINS:
        GPIO.setup(row_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

    # Set up column pins as outputs
    for col_pin in COL_PINS:
        GPIO.setup(col_pin, GPIO.OUT)
        GPIO.output(col_pin, GPIO.HIGH)

    # Set up servo pin as an output
    GPIO.setup(SERVO_PIN, GPIO.OUT)


def init_servo():
    """Initialize servo motor"""
    servo = GPIO.PWM(SERVO_PIN, 50)  # 50 Hz frequency
    servo.start(0)  # Start PWM with 0% duty cycle
    return servo


# Function to move servo to a specified angle
def move_servo(servo, angle):
    """Move servo to specified angle (0-180 degrees)"""
    duty_cycle = (angle / 18) + 2.5
    servo.ChangeDutyCycle(duty_cycle)
    time.sleep(1)  # Allow time for servo movement
    servo.ChangeDutyCycle(0)  # Stop sending signal to prevent jitter


def get_key():
    """Scan keypad and return pressed key"""
    key = None

    # Scan each column
    for col_num, col_pin in enumerate(COL_PINS):
        GPIO.output(col_pin, GPIO.LOW)

        # Check each row
        for row_num, row_pin in enumerate(ROW_PINS):
            if GPIO.input(row_pin) == GPIO.LOW:
                key = KEYPAD[row_num][col_num]

                # Wait for key release to prevent multiple readings
                while GPIO.input(row_pin) == GPIO.LOW:
                    time.sleep(0.05)

        GPIO.output(col_pin, GPIO.HIGH)

    return key


def check_password(input_password):
    """Check if entered password matches any of the valid passwords"""
    return input_password in PASSWORDS


def display_password_status(entered_keys):
    """Display current password entry status"""
    stars = "*" * len(entered_keys)
    print(f"Entered: {stars} ({len(entered_keys)} digits)")


def main():
    """Main program loop"""
    setup_gpio()
    servo = init_servo()
    entered_keys = []

    print("Keypad Security System Started")
    print("Enter password and press '#' to confirm")
    print("Press '*' to clear current entry")
    print("Press Ctrl+C to exit")
    print("-" * 40)

    try:
        while True:
            pressed_key = get_key()

            if pressed_key is not None:
                print(f"Key pressed: {pressed_key}")

                if pressed_key == "*":
                    # Clear current password entry
                    entered_keys = []
                    print("Password cleared")

                elif pressed_key == "#":
                    # Check entered password
                    if len(entered_keys) == 0:
                        print("No password entered. Try again.")
                    elif check_password(entered_keys):
                        print("Password correct! Access granted.")
                        print("Unlocking...")
                        move_servo(servo, 90)  # Move servo to unlock position
                        time.sleep(3)  # Keep unlocked for 3 seconds
                        print("Locking...")
                        move_servo(servo, 0)  # Return servo to lock position
                        entered_keys = []  # Reset password after successful access
                        print("System ready for next password")
                        print("-" * 40)
                    else:
                        print("Incorrect password. Access denied.")
                        entered_keys = []  # Reset password after failed attempt
                        print("Try again")

                else:
                    # Add digit to password (if not at max length)
                    if len(entered_keys) < MAX_PASSWORD_LENGTH:
                        entered_keys.append(pressed_key)
                        display_password_status(entered_keys)
                    else:
                        print(
                            f"Maximum password length ({MAX_PASSWORD_LENGTH}) reached. Press '#' to check or '*' to clear."
                        )

            time.sleep(0.1)  # Small delay to prevent excessive CPU usage

    except KeyboardInterrupt:
        print("\nShutting down...")

    finally:
        # Cleanup GPIO and servo
        servo.stop()
        GPIO.cleanup()
        print("GPIO cleaned up. Goodbye!")


if __name__ == "__main__":
    main()
