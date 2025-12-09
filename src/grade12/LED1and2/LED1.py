import pygame
import sys
import RPi.GPIO as GPIO   # <-- ADDED: GPIO for Raspberry Pi

# Initialize GPIO
LED_GPIO_PIN = 18         # <-- ADDED: LED on GPIO 18
GPIO.setmode(GPIO.BCM)    # <-- ADDED
GPIO.setup(LED_GPIO_PIN, GPIO.OUT, initial=GPIO.LOW)  # <-- ADDED

# Initialize Pygame
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

# Main loop
running = True
while running:
    screen.fill(BG_COLOR)

    # Draw LED
    if led_on:
        pygame.draw.circle(screen, LED_ON_COLOR, LED_POSITION, LED_RADIUS)
    else:
        pygame.draw.circle(screen, LED_OFF_COLOR, LED_POSITION, LED_RADIUS, 2)

    # Instructions for the user
    render_text("Press 1 to turn ON, 0 to turn OFF", (SCREEN_WIDTH // 2, SCREEN_HEIGHT - 60))
    render_text("Press any other key to EXIT", (SCREEN_WIDTH // 2, SCREEN_HEIGHT - 30))

    # Event handling
    for event in pygame.event.get():
