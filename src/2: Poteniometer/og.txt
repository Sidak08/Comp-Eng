#include <Arduino.h>

// Define the analog pin connected to the potentiometer
const float potentiometerPin = 34;
const int ledPin = 21;
float adcValue = 0;

void setup()
{
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
}

void loop()
{
    adcValue = analogRead(potentiometerPin);
    float voltage = adcValue * (3.3 / 4095.0);
    int percentDelay = ((adcValue / 4095) * 100) * 30;
    digitalWrite(ledPin, HIGH);
    delay(percentDelay);
    Serial.println(percentDelay);
    digitalWrite(ledPin, LOW);
    delay(percentDelay);
}
