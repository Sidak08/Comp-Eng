#include <Arduino.h>

// Pin definitions
const int potentiometerPin = 34; // Analog input from potentiometer
const int ledPin = 21;           // LED output
const int buzzerPin = 22;        // Buzzer output
int adcValue = 0;                // Variable to store potentiometer reading

void setup()
{
    Serial.begin(9600);          // Initialize serial communication
    pinMode(ledPin, OUTPUT);     // Set LED pin as output
}

void loop()
{
    // Read the potentiometer value
    adcValue = analogRead(potentiometerPin);
    
    // Map potentiometer reading to frequency range (200-2000 Hz)
    int frequency = map(adcValue, 0, 4095, 200, 2000);
    tone(buzzerPin, frequency);

    // Map potentiometer reading to LED blink rate (1000-100 ms)
    int percentDelay = map(adcValue, 0, 4095, 1000, 100);

    // Blink LED based on mapped delay
    digitalWrite(ledPin, HIGH);
    delay(percentDelay);
    digitalWrite(ledPin, LOW);
    delay(percentDelay);

    // Output values to serial monitor
    Serial.print("Frequency: ");
    Serial.print(frequency);
    Serial.print(" Hz | Delay: ");
    Serial.println(percentDelay);
}
