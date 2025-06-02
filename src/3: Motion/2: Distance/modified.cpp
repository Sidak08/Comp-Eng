#include <Arduino.h>

const int trigPin = 26;
const int echoPin = 27;
const int potPin = 34;

const int redPin = 19;
const int greenPin = 21;
const int bluePin = 18;

const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

float distance = 0;
int potValue = 0;
int threshold1 = 10;
int threshold2 = 20;
int maxDistance = 60;

float getDistance();
void setColor(int red, int green, int blue);
void updateThresholds();

void setup()
{
    Serial.begin(115200);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(potPin, INPUT);

    ledcSetup(redChannel, freq, resolution);
    ledcSetup(greenChannel, freq, resolution);
    ledcSetup(blueChannel, freq, resolution);

    ledcAttachPin(redPin, redChannel);
    ledcAttachPin(greenPin, greenChannel);
    ledcAttachPin(bluePin, blueChannel);
}

void loop()
{
    updateThresholds();
    distance = getDistance();

    if (distance <= threshold1) {
        setColor(255, 0, 0);
    }
    else if (distance > threshold1 && distance < threshold2) {
        setColor(255, 50, 0);
    }
    else {
        setColor(0, 255, 0);
    }

    delay(50);
}

void updateThresholds()
{
    potValue = analogRead(potPin);

    threshold1 = map(potValue, 0, 4095, 1, 30);
    threshold2 = threshold1 + 10;
    if (threshold2 > maxDistance)
    {
        threshold2 = maxDistance;
    }
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
