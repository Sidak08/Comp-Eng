#include <Arduino.h>

// Pin definitions
const int trigPin = 26;
const int echoPin = 27;

const int redPin = 19;
const int greenPin = 21;
const int bluePin = 18;

// PWM settings
const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

float distance = 0;

float getDistance();
void setColor(int red, int green, int blue);

void setup()
{
    Serial.begin(115200);
    Serial.println("ESP32 Ultrasonic Distance Sensor with RGB LED");

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    ledcSetup(redChannel, freq, resolution);
    ledcSetup(greenChannel, freq, resolution);
    ledcSetup(blueChannel, freq, resolution);

    ledcAttachPin(redPin, redChannel);
    ledcAttachPin(greenPin, greenChannel);
    ledcAttachPin(bluePin, blueChannel);
}

void loop()
{
    distance = getDistance();
    if (distance <= 10) {
        setColor(255, 0, 0);
    }
    else if (10 < distance && distance < 20) {
        setColor(255, 50, 0);
    }
    else {
        setColor(0, 255, 0);
    }

    delay(50);
}

void setColor(int red, int green, int blue)
{
    ledcWrite(redChannel, red);
    ledcWrite(greenChannel, green);
    ledcWrite(blueChannel, blue);
}
float getDistance()
{
    float echoTime;
    float calculatedDistance;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    echoTime = pulseIn(echoPin, HIGH);

    calculatedDistance = echoTime / 148.0;

    return calculatedDistance;
}
