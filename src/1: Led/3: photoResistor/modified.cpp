#include <Arduino.h>
#include <ESP32Servo.h>

// Pin definitions
const int photoresistorPin = 32; // Analog input from photoresistor
const int ledPin = 13;           // Digital output to LED
const int servoPin = 26;         // Servo control pin

// Threshold setting
int threshold = 700; // Light threshold for LED control

// Variables for readings
int photoresistorValue = 0; // Raw photoresistor reading
int servoPosition = 0;      // Servo position (0-180 degrees)

// Servo settings
int minServoAngle = 0;    // Minimum servo angle
int maxServoAngle = 180;  // Maximum servo angle
int minLightValue = 500;  // Expected minimum photoresistor reading
int maxLightValue = 4000; // Expected maximum photoresistor reading

// Create servo object
Servo myServo;

void setup()
{
    Serial.begin(9600);
    delay(1000);        // Wait for serial to initialize

    // Configure pins
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW); // Start with LED off

    // Attach servo
    myServo.attach(servoPin);
    myServo.write(90); // Start at center position
    delay(500);

    Serial.println("Photoresistor LED and Servo Controller Ready");
}

void loop()
{
    // Read the photoresistor
    photoresistorValue = analogRead(photoresistorPin);

    Serial.println(photoresistorValue);

    // Control LED based on threshold
    if (photoresistorValue < threshold)
    {
        digitalWrite(ledPin, HIGH); // Turn on LED when darker
    }
    else
    {
        digitalWrite(ledPin, LOW); // Turn off LED when brighter
    }

    // Map photoresistor value to servo position
    servoPosition = map(photoresistorValue, minLightValue, maxLightValue, minServoAngle, maxServoAngle);
    servoPosition = constrain(servoPosition, minServoAngle, maxServoAngle);

    myServo.write(servoPosition);

    delay(100);
}
