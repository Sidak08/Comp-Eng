// ESP32 Button Trumpet - Play different tones with button presses

#include <Arduino.h>

// Pin definitions
int firstKeyPin = 13;  // First button
int secondKeyPin = 33; // Second button
int thirdKeyPin = 14;  // Third button

int buzzerPin = 25;    // Buzzer output

void playTone(int freq)
{
    ledcSetup(0, freq, 8);       // Configure PWM channel
    ledcAttachPin(buzzerPin, 0); // Connect buzzer to PWM channel
    ledcWrite(0, 127);           // 50% duty cycle
}

void stopTone()
{
    ledcDetachPin(buzzerPin);    // Disconnect buzzer from PWM
}

void setup()
{
    // Configure button pins with pull-up resistors
    pinMode(firstKeyPin, INPUT_PULLUP);
    pinMode(secondKeyPin, INPUT_PULLUP);
    pinMode(thirdKeyPin, INPUT_PULLUP);

    // Configure buzzer pin as output
    pinMode(buzzerPin, OUTPUT);
    Serial.begin(9600);
    Serial.println("ESP32 started");
}

void loop()
{
    // Check which button is pressed and play corresponding tone
    if (digitalRead(firstKeyPin) == LOW)
    {
        playTone(262); // Play C note
        Serial.println("C");
    }
    else if (digitalRead(secondKeyPin) == LOW)
    {
        playTone(330); // Play E note
        Serial.println("E");
    }
    else if (digitalRead(thirdKeyPin) == LOW)
    {
        playTone(392); // Play G note
        Serial.println("G");
    }
    else
    {
        stopTone(); // Stop sound when no buttons are pressed
    }
}

// Note frequencies in Hz: c=262, d=294, e=330, f=349, g=392, a=440, b=494, C=523
