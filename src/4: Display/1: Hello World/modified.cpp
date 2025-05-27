// ESP32 Hello World LCD with Button Counter
// Displays "Hello World" and increments a counter when button is pressed
#include <Arduino.h>
#include <LiquidCrystal.h> // Library for controlling LCD display

// LCD pin connections (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

// Button pin
const int buttonPin = 22;

// Variables
int buttonState = HIGH;     // Current button state
int lastButtonState = HIGH; // Previous button state
int counter = 0;            // Button press counter

void setup()
{
    lcd.begin(16, 2);
    lcd.clear();

    pinMode(buttonPin, INPUT_PULLUP);

    // Initial display
    lcd.setCursor(0, 0);
    lcd.print("Hello, world!");

    lcd.setCursor(0, 1);
    lcd.print("Counter: ");
    lcd.print(counter);
}

void loop()
{
    buttonState = digitalRead(buttonPin);

    // Detect button press (transition from HIGH to LOW)
    if (buttonState == LOW && lastButtonState == HIGH)
    {
        counter++;

        // Update counter on LCD
        lcd.setCursor(9, 1);
        lcd.print(counter);
        lcd.print("      "); // Clear extra digits
    }

    lastButtonState = buttonState;
    delay(50); // Debounce delay
}
