#include <Arduino.h>
#include <LiquidCrystal.h>

const int rs = 13; // Register Select
const int en = 12; // Enable
const int d4 = 14; // Data pin 4
const int d5 = 27; // Data pin 5
const int d6 = 26; // Data pin 6
const int d7 = 25; // Data pin 7

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
    Serial.begin(9600);
    lcd.begin(16, 2);

    lcd.clear();
    for (int i = 0; i < 32; i++)
    {
        lcd.write(255);
    }
    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hello World");
    lcd.setCursor(0, 1);
    lcd.print("Hello World");
}

void loop()
{
    static unsigned long lastUpdate = 0;
    static int counter = 0;

    if (millis() - lastUpdate >= 1000)
    {
        lastUpdate = millis();
        counter++;

        lcd.setCursor(0, 1);
        lcd.print("Counter: ");
        lcd.print(counter);
        lcd.print("   ");
    }
}
