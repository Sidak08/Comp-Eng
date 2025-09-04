#include <Arduino.h>


const float potentiometerPin = 34; // potentiometer
const int ledPin = 21;             // LED
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
