#include <Arduino.h>

// Pins
const int photoresistorPin = 32;
const int potentiometerPin = 34;
const int RedPin = 22;
const int GreenPin = 23;
const int BluePin = 21;

// photoresistor
const int threshold = 3500;

// PWM settings
const int pwmFreq = 5000;
const int pwmResolution = 8; // 0-255
const int RedChannel = 0;
const int GreenChannel = 1;
const int BlueChannel = 2;

void red();
void orange();
void yellow();
void green();
void cyan();
void blue();
void magenta();
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

}

void loop()
{
    // Read sensors
    int photoresistor = analogRead(photoresistorPin);
    int potentiometer = analogRead(potentiometerPin);


    if (photoresistor > threshold)
    {
        // Dark environment - turn on LED
        if (potentiometer > 0 && potentiometer <= 150)
        {
            red();
        }
        else if (potentiometer > 150 && potentiometer <= 300)
        {
            orange();
        }
        else if (potentiometer > 300 && potentiometer <= 450)
        {
            yellow();
        }
        else if (potentiometer > 450 && potentiometer <= 600)
        {
            green();
        }
        else if (potentiometer > 600 && potentiometer <= 750)
        {
            cyan();
        }
        else if (potentiometer > 750 && potentiometer <= 900)
        {
            blue();
        }
        else if (potentiometer > 900)
        {
            magenta();
        }
    }
    else
    {
        turnOff();
    }

    delay(100);
}

// Color functions
void red()
{
    ledcWrite(RedChannel, 255);
    ledcWrite(GreenChannel, 0);
    ledcWrite(BlueChannel, 0);
}

void orange()
{
    ledcWrite(RedChannel, 255);
    ledcWrite(GreenChannel, 128);
    ledcWrite(BlueChannel, 0);
}

void yellow()
{
    ledcWrite(RedChannel, 255);
    ledcWrite(GreenChannel, 255);
    ledcWrite(BlueChannel, 0);
}

void green()
{
    ledcWrite(RedChannel, 0);
    ledcWrite(GreenChannel, 255);
    ledcWrite(BlueChannel, 0);
}

void cyan()
{
    ledcWrite(RedChannel, 0);
    ledcWrite(GreenChannel, 255);
    ledcWrite(BlueChannel, 255);
}

void blue()
{
    ledcWrite(RedChannel, 0);
    ledcWrite(GreenChannel, 0);
    ledcWrite(BlueChannel, 255);
}

void magenta()
{
    ledcWrite(RedChannel, 255);
    ledcWrite(GreenChannel, 0);
    ledcWrite(BlueChannel, 255);
}

void turnOff()
{
    ledcWrite(RedChannel, 0);
    ledcWrite(GreenChannel, 0);
    ledcWrite(BlueChannel, 0);
}
