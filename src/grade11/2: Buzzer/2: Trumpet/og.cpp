#include <Arduino.h>

// Pin definitions
int firstKeyPin = 13;  // First button
int secondKeyPin = 33; // Second button
int thirdKeyPin = 14;  // Third button

int buzzerPin = 25;    // Buzzer output

void playTone(int freq)
{
    ledcSetup(0, freq, 8);
    ledcAttachPin(buzzerPin, 0);
    ledcWrite(0, 127);
}

void stopTone()
{
    ledcDetachPin(buzzerPin);
}

void setup()
{
    pinMode(firstKeyPin, INPUT_PULLUP);
    pinMode(secondKeyPin, INPUT_PULLUP);
    pinMode(thirdKeyPin, INPUT_PULLUP);

    pinMode(buzzerPin, OUTPUT);
    Serial.begin(9600);
}

void loop()
{
    if (digitalRead(firstKeyPin) == LOW)
    {
        playTone(262);
        Serial.println("C");
    }
    else if (digitalRead(secondKeyPin) == LOW)
    {
        playTone(330);
        Serial.println("E");
    }
    else if (digitalRead(thirdKeyPin) == LOW)
    {
        playTone(392);
        Serial.println("G");
    }
    else
    {
        stopTone();
    }
}
