import sys

import pygame
import RPi.GPIO as GPIO

# ---------- GPIO SETUP ----------
LED_GPIO_PIN = 18  # LED pin
BUTTON_GPIO_PIN = 23  # Button pin

GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_GPIO_PIN, GPIO.OUT, initial=GPIO.LOW)

# Assume button wired to GND with internal pull-up
GPIO.setup(BUTTON_GPIO_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# ---------- PYGAME SETUP ----------
pygame.init()

# Screen dimensions and settings
SCREEN_WIDTH = 400
SCREEN_HEIGHT = 300
BG_COLOR = (30, 30, 30)
LED_OFF_COLOR = (200, 200, 200)
LED_ON_COLOR = (255, 0, 0)
TEXT_COLOR = (255, 255, 255)

# Create the screen
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("LED Control")

# Font setup
font = pygame.font.Font(None, 36)

# LED position and radius
LED_POSITION = (SCREEN_WIDTH // 2, SCREEN_HEIGHT // 2)
LED_RADIUS = 40

# LED state
led_on = False


# Function to render text on the screen
def render_text(text, position):
    text_surface = font.render(text, True, TEXT_COLOR)
    text_rect = text_surface.get_rect(center=position)
    screen.blit(text_surface, text_rect)


running = True
try:
    while running:
        screen.fill(BG_COLOR)

        # ----- BUTTON CONTROLS LED DIRECTLY (momentary) -----
        button_pressed = GPIO.input(BUTTON_GPIO_PIN) == GPIO.LOW
        led_on = button_pressed
        GPIO.output(LED_GPIO_PIN, GPIO.HIGH if led_on else GPIO.LOW)

        # Draw LED
        if led_on:
            pygame.draw.circle(screen, LED_ON_COLOR, LED_POSITION, LED_RADIUS)
        else:
            pygame.draw.circle(screen, LED_OFF_COLOR, LED_POSITION, LED_RADIUS, 2)

        # GUI text (unchanged except we removed the 1/0 line)
        render_text(
            "Press any other key to EXIT", (SCREEN_WIDTH // 2, SCREEN_HEIGHT - 30)
        )

        # Event handling
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                # ANY KEY ends the program
                running = False

        # Update the screen
        pygame.display.flip()

finally:
    pygame.quit()
    GPIO.cleanup()
    sys.exit()
