#include <Arduino.h>


const int potentiometerPin = 34; // potentiometer
const int ledPin = 21;           // LED
const int buzzerPin = 22;        // Buzzer
int adcValue = 0;                // potentiometer reading

void setup()
{
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
}

void loop()
{
    adcValue = analogRead(potentiometerPin);

    int frequency = map(adcValue, 0, 4095, 200, 2000);
    tone(buzzerPin, frequency);

    int percentDelay = map(adcValue, 0, 4095, 1000, 100);

    digitalWrite(ledPin, HIGH);
    delay(percentDelay);
    digitalWrite(ledPin, LOW);
    delay(percentDelay);

    Serial.print("Frequency: ");
    Serial.print(frequency);
    Serial.print(" Hz | Delay: ");
    Serial.println(percentDelay);
}
