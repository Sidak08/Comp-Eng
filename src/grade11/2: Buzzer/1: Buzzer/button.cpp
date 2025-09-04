#include <Arduino.h>

const int buttonPin = 13;

void setup()
{
    pinMode(buttonPin, INPUT);
}

void loop()
{
    int state = digitalRead(buttonPin);

}
