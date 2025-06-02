#include <Arduino.h>

const int redPin = 23;
const int bluePin = 22;
const int greenPin = 21;
const int whiteLedPin = 19;

void setup()
{
    pinMode(redPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(whiteLedPin, OUTPUT);
}

void loop()
{

    digitalWrite(whiteLedPin, HIGH);
    delay(500);
    digitalWrite(redPin, HIGH);
    delay(500);
    digitalWrite(redPin, LOW);
    delay(500);
    digitalWrite(whiteLedPin, LOW);
    delay(500);


    digitalWrite(whiteLedPin, HIGH);
    delay(500);
    digitalWrite(greenPin, HIGH);
    delay(500);
    digitalWrite(greenPin, LOW);
    delay(500);
    digitalWrite(whiteLedPin, LOW);
    delay(500);


    digitalWrite(whiteLedPin, HIGH);
    delay(500);
    digitalWrite(bluePin, HIGH);
    delay(500);
    digitalWrite(bluePin, LOW);
    delay(500);
    digitalWrite(whiteLedPin, LOW);
    delay(500);
}
