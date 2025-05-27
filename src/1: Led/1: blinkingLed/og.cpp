#include <Arduino.h>

// Pin definition for LED
const int ledPin = 23;

void setup()
{
  pinMode(ledPin, OUTPUT); // Set LED pin as output
}

void loop()
{
  digitalWrite(ledPin, HIGH); // Turn the LED on
  delay(500);                 // Wait for 500 milliseconds

  digitalWrite(ledPin, LOW);  // Turn the LED off
  delay(500);                 // Wait for 500 milliseconds
}
