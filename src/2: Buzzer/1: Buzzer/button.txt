/*
  SparkFun Inventor’s Kit
  Circuit 2A - Buzzer

  Play notes using a buzzer connected to pin 10

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v40
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/

#include <Arduino.h>

const int buttonPin = 13;

void setup()
{
    Serial.begin(9600);
    Serial.println("ESP32 started");
    pinMode(buttonPin, INPUT);
}

void loop()
{
    int state = digitalRead(buttonPin);
    Serial.println(state);
    delay(50);
}
