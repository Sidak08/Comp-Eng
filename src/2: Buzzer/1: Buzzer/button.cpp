#include <Arduino.h>

const int buttonPin = 13; // Pin for button input

void setup()
{
    Serial.begin(9600);
    Serial.println("ESP32 started");
    pinMode(buttonPin, INPUT);
}

void loop()
{
    int state = digitalRead(buttonPin); // Read button state
    Serial.println(state);
    delay(50);
}
