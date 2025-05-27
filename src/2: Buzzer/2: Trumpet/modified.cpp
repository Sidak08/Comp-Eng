#include <Arduino.h>

// button pins
int firstKeyPin = 13;
int secondKeyPin = 33;
int thirdKeyPin = 14;

// buzzer pins
int buzzer1Pin = 25;
int buzzer2Pin = 4;

// channel numbers (each must be unique)
int buzzer1Channel = 0;
int buzzer2Channel = 1;

void playTone(int freq1, int freq2)
{
    // Buzzer 1 (pin 25)
    ledcSetup(buzzer1Channel, freq1, 8); // Channel 0
    ledcAttachPin(buzzer1Pin, buzzer1Channel);
    ledcWrite(buzzer1Channel, 127); // 50% duty cycle

    // Buzzer 2 (pin 4)
    ledcSetup(buzzer2Channel, freq2, 8); // Channel 1
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
    pinMode(firstKeyPin, INPUT_PULLUP);
    pinMode(secondKeyPin, INPUT_PULLUP);
    pinMode(thirdKeyPin, INPUT_PULLUP);

    pinMode(buzzer1Pin, OUTPUT);
    pinMode(buzzer2Pin, OUTPUT);

    Serial.begin(9600);
    Serial.println("ESP32 started");
}

void loop()
{
    if (digitalRead(firstKeyPin) == LOW)
    {
        playTone(262, 392); // C + G
        Serial.println("C + G");
    }
    else if (digitalRead(secondKeyPin) == LOW)
    {
        playTone(330, 440); // E + A
        Serial.println("E + A");
    }
    else if (digitalRead(thirdKeyPin) == LOW)
    {
        playTone(392, 523); // G + C (octave)
        Serial.println("G + C");
    }
    else
    {
        stopTones();
    }
}
