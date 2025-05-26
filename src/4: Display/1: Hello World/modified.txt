/*
  SparkFun Inventor's Kit - Modified for ESP32
  Circuit 4A-HelloWorld with Button Counter

  The LCD will display the words "Hello World" and a counter that
  increments each time a button is pressed.

  Adapted for ESP32 from original SparkFun Electronics code.
*/
#include <Arduino.h>
#include <LiquidCrystal.h> // The liquid crystal library contains commands for printing to the display

// Define the pins connected to the LCD for ESP32
// RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 14, 27, 26, 25); // Tell the ESP32 what pins are connected to the display

// Define the button pin
const int buttonPin = 22; // Button connected to GPIO 22

// Variable to store button state and counter
int buttonState = HIGH;     // Current button state
int lastButtonState = HIGH; // Previous button state
int counter = 0;            // Counter variable

void setup()
{
    lcd.begin(16, 2); // Tell the lcd library that we're using a 16x2 display
    lcd.clear();      // Clear the display

    pinMode(buttonPin, INPUT_PULLUP); // Set button pin as input with internal pull-up resistor

    // Display initial message
    lcd.setCursor(0, 0);        // Set cursor to top left
    lcd.print("Hello, world!"); // Print greeting

    lcd.setCursor(0, 1);    // Set cursor to bottom left
    lcd.print("Counter: "); // Print label
    lcd.print(counter);     // Print initial counter value
}

void loop()
{
    // Read the button state
    buttonState = digitalRead(buttonPin);

    // Check if the button was pressed (transition from HIGH to LOW)
    if (buttonState == LOW && lastButtonState == HIGH)
    {
        // Button was pressed, increment counter
        counter++;

        // Update the counter on the LCD
        lcd.setCursor(9, 1); // Position cursor after "Counter: "
        lcd.print(counter);  // Print the updated counter
        lcd.print("      "); // Clear any remaining characters
    }

    // Save the current button state for the next loop iteration
    lastButtonState = buttonState;

    // Small delay for button debouncing
    delay(50);
}
