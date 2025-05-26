#include <Arduino.h>
#include <LiquidCrystal.h>

// Define the pins connected to the LCD
const int rs = 13; // Register Select pin
const int en = 12; // Enable pin
const int d4 = 14; // Data pin 4
const int d5 = 27; // Data pin 5
const int d6 = 26; // Data pin 6
const int d7 = 25; // Data pin 7

// Initialize the LiquidCrystal object with the ESP32 pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
    // Start Serial for debugging
    Serial.begin(9600);
    Serial.println("ESP32 LCD Test");

    // Initialize the LCD
    lcd.begin(16, 2);

    // Test the display with different patterns
    // This helps identify if the display is working but contrast is wrong

    // Fill the screen with blocks (all pixels on)
    lcd.clear();
    for (int i = 0; i < 32; i++)
    {
        lcd.write(255); // Character code 255 is a solid block on most LCD displays
    }
    Serial.println("Display filled with blocks");
    delay(2000);

    // Clear and show text
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hello World");
    lcd.setCursor(0, 1);
    lcd.print("Hello World");
    Serial.println("Test message displayed");
}

void loop()
{
    // In the loop, we'll show a counter to verify the display is updating
    static unsigned long lastUpdate = 0;
    static int counter = 0;

    // Update counter every second
    if (millis() - lastUpdate >= 1000)
    {
        lastUpdate = millis();
        counter++;

        lcd.setCursor(0, 1);
        lcd.print("Counter: ");
        lcd.print(counter);
        lcd.print("   "); // Clear any remaining characters

        Serial.print("Counter updated: ");
        Serial.println(counter);
    }
}
