#include <Arduino.h>
#include <ESP32Servo.h>

// Pin definitions
const int trigPin = 26;
const int echoPin = 27;
const int redPin = 19;
const int greenPin = 21;
const int bluePin = 18;
const int buzzerPin = 25;
const int servoPin = 23;

// PWM settings
const int freq = 5000;
const int buzzerChannel = 3;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

float distance = 0;

Servo myservo;
float getDistance();
void setColor(int red, int green, int blue);

void setup()
{
  Serial.begin(115200);
  Serial.println("ESP32 Motion Alarm System");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);
  ledcSetup(buzzerChannel, 2000, resolution);

  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  ledcAttachPin(buzzerPin, buzzerChannel);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);

  myservo.write(90);
  delay(500);
}

void loop() {
  distance = getDistance();

  if (distance <= 10) {
    setColor(255, 0, 0);

    ledcWriteTone(buzzerChannel, 272);
    myservo.write(10);
    delay(100);

    ledcWrite(buzzerChannel, 0);
    myservo.write(150);
    delay(100);
  }
  else if (10 < distance && distance < 20) {
    // Medium distance - yellow warning
    setColor(255, 50, 0);
  }
  else {
    // Far distance - safe condition
    setColor(0, 255, 0);
  }

  delay(50);
}

void setColor(int red, int green, int blue) {
  ledcWrite(redChannel, red);
  ledcWrite(greenChannel, green);
  ledcWrite(blueChannel, blue);
}

float getDistance() {
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
