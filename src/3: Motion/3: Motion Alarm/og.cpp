// ESP32 Motion Alarm System
// Detects proximity with ultrasonic sensor and activates alarm with
// LED, buzzer, and servo motor movements

#include <Arduino.h>
#include <ESP32Servo.h>  // ESP32 servo library

// Pin definitions
const int trigPin = 26;     // Ultrasonic sensor trigger
const int echoPin = 27;     // Ultrasonic sensor echo
const int redPin = 19;      // RGB LED red pin
const int greenPin = 21;    // RGB LED green pin
const int bluePin = 18;     // RGB LED blue pin
const int buzzerPin = 25;   // Buzzer output
const int servoPin = 23;    // Servo control

// PWM settings
const int freq = 5000;      // PWM frequency
const int buzzerChannel = 3;// Buzzer PWM channel
const int redChannel = 0;   // Red PWM channel
const int greenChannel = 1; // Green PWM channel
const int blueChannel = 2;  // Blue PWM channel
const int resolution = 8;   // 8-bit (0-255)

float distance = 0;         // Measured distance

Servo myservo;

// Function prototypes
float getDistance();
void setColor(int red, int green, int blue);

void setup()
{
  Serial.begin(115200);
  Serial.println("ESP32 Motion Alarm System");

  pinMode(trigPin, OUTPUT); // Configure trigger as output
  pinMode(echoPin, INPUT);  // Configure echo as input
  
  // Initialize PWM channels
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);
  ledcSetup(buzzerChannel, 2000, resolution);
  
  // Connect PWM channels to pins
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  ledcAttachPin(buzzerPin, buzzerChannel);
  
  // Initialize servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);
  
  // Center the servo initially
  myservo.write(90);
  delay(500);
}

void loop() {
  distance = getDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" in");

  if (distance <= 10) {
    // Object is close - activate alarm
    setColor(255, 0, 0);

    // Sound buzzer and move servo
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

// Core functions

// Set RGB LED color
void setColor(int red, int green, int blue) {
  ledcWrite(redChannel, red);
  ledcWrite(greenChannel, green);
  ledcWrite(blueChannel, blue);
}

// Measure distance using ultrasonic sensor
float getDistance() {
  float echoTime;
  float calculatedDistance;
  
  // Send trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Get echo response
  echoTime = pulseIn(echoPin, HIGH);
  
  // Convert to inches
  calculatedDistance = echoTime / 148.0;
  
  return calculatedDistance;
}
