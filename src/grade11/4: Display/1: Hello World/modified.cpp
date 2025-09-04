#include <Arduino.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

const int buttonPin = 22;

// Variables
int buttonState = HIGH;
int lastButtonState = HIGH;
int counter = 0;

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
    if (buttonState == LOW && lastButtonState == HIGH)
    {
        counter++;

        // Update counter on LCD
        lcd.setCursor(9, 1);
        lcd.print(counter);
        lcd.print("      ");
    }

    lastButtonState = buttonState;
    delay(50); // Debounce delay
}
