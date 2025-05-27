/*
  SparkFun Inventor’s Kit
  Circuit 2B-ButtonTrumpet

  Use 3 buttons plugged to play musical notes on a buzzer.

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v40
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/

#include <Arduino.h>

// set the pins for the button and buzzer
int firstKeyPin = 13;
int secondKeyPin = 33;
int thirdKeyPin = 14;

int buzzerPin = 25;

void playTone(int freq)
{
    ledcSetup(0, freq, 8);       // Channel 0, frequency, 8-bit resolution
    ledcAttachPin(buzzerPin, 0); // Attach buzzer pin to channel 0
    ledcWrite(0, 127);           // 50% duty cycle (127/255)
}

void stopTone()
{
    ledcDetachPin(buzzerPin);
}

void setup()
{
    // set the button pins as inputs
    pinMode(firstKeyPin, INPUT_PULLUP);
    pinMode(secondKeyPin, INPUT_PULLUP);
    pinMode(thirdKeyPin, INPUT_PULLUP);

    // set the buzzer pin as an output
    pinMode(buzzerPin, OUTPUT);
    Serial.begin(9600);
    Serial.println("ESP32 started");
}

void loop()
{
    if (digitalRead(firstKeyPin) == LOW)
    {
        playTone(262); // C
        Serial.println("C");
    }
    else if (digitalRead(secondKeyPin) == LOW)
    {
        playTone(330); // E
        Serial.println("E");
    }
    else if (digitalRead(thirdKeyPin) == LOW)
    {
        playTone(392); // G
        Serial.println("G");
    }
    else
    {
        stopTone();
    }
}

/*
  note  frequency
  c     262 Hz
  d     294 Hz
  e     330 Hz
  f     349 Hz
  g     392 Hz
  a     440 Hz
  b     494 Hz
  C     523 Hz
*/
