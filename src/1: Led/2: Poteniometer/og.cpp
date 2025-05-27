#include <Arduino.h>

// Pin definitions
const float potentiometerPin = 34; // Analog input from potentiometer
const int ledPin = 21;             // LED output
float adcValue = 0;                // Variable to store potentiometer reading

void setup()
{
    Serial.begin(9600);          // Initialize serial communication
    pinMode(ledPin, OUTPUT);     // Set LED pin as output
}

void loop()
{
    // Read the potentiometer value
    adcValue = analogRead(potentiometerPin);
    
    // Convert ADC reading to voltage (0-3.3V)
    float voltage = adcValue * (3.3 / 4095.0);
    
    // Calculate delay time based on potentiometer position
    int percentDelay = ((adcValue / 4095) * 100) * 30;
    
    // Blink LED based on calculated delay
    digitalWrite(ledPin, HIGH);
    delay(percentDelay);
    Serial.println(percentDelay);
    digitalWrite(ledPin, LOW);
    delay(percentDelay);
}
