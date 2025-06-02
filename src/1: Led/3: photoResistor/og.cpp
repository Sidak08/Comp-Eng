#include <Arduino.h>


const int photoresistorPin = 32; // photoresistor
const int ledPin = 13;           // LED

// Threshold setting
int threshold = 600;

// Variable for reading
int photoresistorValue = 0;

void setup()
{
    Serial.begin(9600);
    delay(1000);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    Serial.println("Photoresistor LED Controller Ready");
}

void loop()
{
    photoresistorValue = analogRead(photoresistorPin);

    Serial.println(photoresistorValue);

    if (photoresistorValue < threshold)
    {
        digitalWrite(ledPin, HIGH);
    }
    else
    {
        digitalWrite(ledPin, LOW);
    }

    delay(100);
}
