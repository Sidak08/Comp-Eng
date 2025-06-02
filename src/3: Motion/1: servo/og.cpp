#include <Arduino.h>
#include <ESP32Servo.h>

// Pin definitions
const int potPin = 34;
const int servoPin = 23;

int potPosition;
int servoPosition;

Servo myservo;

void setup()
{
    Serial.begin(115200);

    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    myservo.setPeriodHertz(50);
    myservo.attach(servoPin, 500, 2400);
}

void loop()
{
    potPosition = analogRead(potPin);
    servoPosition = map(potPosition, 0, 4095, 20, 160);

    myservo.write(servoPosition);

    delay(15);
}
