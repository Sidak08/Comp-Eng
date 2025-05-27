#include <Arduino.h>

// Pin definitions
int firstKeyPin = 13;  // First button
int secondKeyPin = 33; // Second button
int thirdKeyPin = 14;  // Third button

int buzzer1Pin = 25;   // First buzzer output
int buzzer2Pin = 4;    // Second buzzer output

// PWM channel assignments
int buzzer1Channel = 0;
int buzzer2Channel = 1;

void playTone(int freq1, int freq2)
{
    // Configure and start first buzzer
    ledcSetup(buzzer1Channel, freq1, 8);
    ledcAttachPin(buzzer1Pin, buzzer1Channel);
    ledcWrite(buzzer1Channel, 127); // 50% duty cycle

    // Configure and start second buzzer
    ledcSetup(buzzer2Channel, freq2, 8);
    ledcAttachPin(buzzer2Pin, buzzer2Channel);
    ledcWrite(buzzer2Channel, 127); // 50% duty cycle
}

void stopTones()
{
    ledcDetachPin(buzzer1Pin);
    ledcDetachPin(buzzer2Pin);
}

void setup()
{
    // Configure button pins with pull-up resistors
    pinMode(firstKeyPin, INPUT_PULLUP);
    pinMode(secondKeyPin, INPUT_PULLUP);
    pinMode(thirdKeyPin, INPUT_PULLUP);

    // Configure buzzer pins as outputs
    pinMode(buzzer1Pin, OUTPUT);
    pinMode(buzzer2Pin, OUTPUT);

    Serial.begin(9600);
    Serial.println("ESP32 started");
}

void loop()
{
    // Check which button is pressed and play corresponding tones
    if (digitalRead(firstKeyPin) == LOW)
    {
        playTone(262, 392); // Play C + G chord
        Serial.println("C + G");
    }
    else if (digitalRead(secondKeyPin) == LOW)
    {
        playTone(330, 440); // Play E + A chord
        Serial.println("E + A");
    }
    else if (digitalRead(thirdKeyPin) == LOW)
    {
        playTone(392, 523); // Play G + C chord (octave)
        Serial.println("G + C");
    }
    else
    {
        stopTones(); // Stop sound when no buttons are pressed
    }
}
