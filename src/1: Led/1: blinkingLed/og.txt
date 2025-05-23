#include <Arduino.h>


const int ledPin = 23;

void setup()
{
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  // Turn the LED on
  digitalWrite(ledPin, HIGH);
  delay(500);

  digitalWrite(ledPin, LOW);
  delay(500);
}
