#include <Arduino.h>

// Pins
const int photoresistorPin = 32;
const int potentiometerPin = 34;
const int RedPin = 22;
const int GreenPin = 23;
const int BluePin = 21;
const int signalPin = 5;     

// photoresistor
const int threshold = 3500;

// PWM settings
const int pwmFreq = 5000;
const int pwmResolution = 8; // 0-255
const int RedChannel = 0;
const int GreenChannel = 1;
const int BlueChannel = 2;

// Function declaration
void turnOff();

void setup()
{
    Serial.begin(9600);

    ledcSetup(RedChannel, pwmFreq, pwmResolution);
    ledcAttachPin(RedPin, RedChannel);

    ledcSetup(GreenChannel, pwmFreq, pwmResolution);
    ledcAttachPin(GreenPin, GreenChannel);

    ledcSetup(BlueChannel, pwmFreq, pwmResolution);
    ledcAttachPin(BluePin, BlueChannel);

    pinMode(signalPin, OUTPUT);
    digitalWrite(signalPin, LOW);

}

void loop()
{
    int photoresistor = analogRead(photoresistorPin);
    int potentiometer = analogRead(potentiometerPin);

    if (photoresistor > threshold)
    {
        digitalWrite(signalPin, LOW);
        int redValue = map(potentiometer, 0, 4095, 255, 0);
        int greenValue = map(potentiometer, 0, 4095, 0, 255);
        int blueValue = map(potentiometer, 2048, 4095, 0, 255);

        if (potentiometer < 2048)
        {
            blueValue = 0;
        }

        ledcWrite(RedChannel, redValue);
        ledcWrite(GreenChannel, greenValue);
        ledcWrite(BlueChannel, blueValue);
    }
    else
    {
        turnOff();
        digitalWrite(signalPin, HIGH);
    }

    delay(100);
}

void turnOff()
{
    ledcWrite(RedChannel, 0);
    ledcWrite(GreenChannel, 0);
    ledcWrite(BlueChannel, 0);
}
