#include <Arduino.h>

// Pin definitions
const int photoresistorPin = 32; // Analog input from photoresistor
const int ledPin = 13;           // Digital output to LED

// Threshold setting
int threshold = 600; // Adjust based on your environment

// Variable for reading
int photoresistorValue = 0; // Raw photoresistor reading

void setup()
{
    Serial.begin(9600);        // Match this to your terminal (9600)
    delay(1000);               // Wait for serial to initialize
    pinMode(ledPin, OUTPUT);   // Configure LED pin as output
    digitalWrite(ledPin, LOW); // Start with LED off
    Serial.println("Photoresistor LED Controller Ready");
}

void loop()
{
    // Read the photoresistor
    photoresistorValue = analogRead(photoresistorPin);

    // Print the current reading
    Serial.println(photoresistorValue);

    // Control LED based on threshold
    if (photoresistorValue < threshold)
    {
        digitalWrite(ledPin, HIGH); // Turn on LED when darker than threshold
    }
    else
    {
        digitalWrite(ledPin, LOW); // Turn off LED when brighter than threshold
    }

    delay(100); // Short delay for stability
}
