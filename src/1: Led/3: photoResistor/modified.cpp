#include <Arduino.h>
#include <ESP32Servo.h>

const int photoresistorPin = 32; // photoresistor
const int ledPin = 13;           // LED
const int servoPin = 26;         // Servo

int threshold = 700;

int photoresistorValue = 0;
int servoPosition = 0;

int minServoAngle = 0;
int maxServoAngle = 180;
int minLightValue = 500;
int maxLightValue = 4000;

Servo myServo;

void setup()
{
    Serial.begin(9600);
    delay(1000);

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);


    myServo.attach(servoPin);
    myServo.write(90);
    delay(500);
}

void loop()
{
    photoresistorValue = analogRead(photoresistorPin);

    Serial.println(photoresistorValue);

    if (photoresistorValue < threshold)
    {
        digitalWrite(ledPin, HIGH); 
    }
    else
    {
        digitalWrite(ledPin, LOW);
    }

    servoPosition = map(photoresistorValue, minLightValue, maxLightValue, minServoAngle, maxServoAngle);
    servoPosition = constrain(servoPosition, minServoAngle, maxServoAngle);

    myServo.write(servoPosition);

    delay(100);
}
