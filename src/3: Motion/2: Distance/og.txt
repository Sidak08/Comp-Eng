/*
  ESP32 Distance Sensor with RGB LED

  Control the color of an RGB LED using an ultrasonic distance sensor.
  Adapted from SparkFun Inventor's Kit for ESP32.
*/

#include <Arduino.h>

// Pin definitions for ESP32
const int trigPin = 26; // Trigger pin for HC-SR04
const int echoPin = 27; // Echo pin for HC-SR04

const int redPin = 19;   // Controls red LED
const int greenPin = 21; // Controls green LED
const int bluePin = 18;  // Controls blue LED

// PWM properties
const int freq = 5000;      // 5000 Hz PWM frequency
const int redChannel = 0;   // PWM channel for red
const int greenChannel = 1; // PWM channel for green
const int blueChannel = 2;  // PWM channel for blue
const int resolution = 8;   // 8-bit resolution (0-255)

float distance = 0; // Stores the distance measured by the distance sensor

// Function declarations - these must appear before they're called
float getDistance();
void setColor(int red, int green, int blue);

void setup()
{
    Serial.begin(115200); // Higher baud rate for ESP32
    Serial.println("ESP32 Ultrasonic Distance Sensor with RGB LED");

    pinMode(trigPin, OUTPUT); // Trigger pin outputs pulses
    pinMode(echoPin, INPUT);  // Echo pin measures returning pulses

    // Configure PWM for RGB LED
    ledcSetup(redChannel, freq, resolution);
    ledcSetup(greenChannel, freq, resolution);
    ledcSetup(blueChannel, freq, resolution);

    // Attach the PWM channels to the GPIO pins
    ledcAttachPin(redPin, redChannel);
    ledcAttachPin(greenPin, greenChannel);
    ledcAttachPin(bluePin, blueChannel);
}

void loop()
{
    distance = getDistance(); // Get distance from sensor

    Serial.print(distance); // Print the distance
    Serial.println(" in");  // Print units

    if (distance <= 10)
    { // Object is close
        // Make the RGB LED red
        setColor(255, 0, 0);
    }
    else if (10 < distance && distance < 20)
    { // Medium distance
        // Make the RGB LED yellow
        setColor(255, 50, 0);
    }
    else
    { // Object is far away
        // Make the RGB LED green
        setColor(0, 255, 0);
    }

    delay(50); // Delay between readings
}

//------------------FUNCTIONS-------------------------------

// Set the RGB LED color
void setColor(int red, int green, int blue)
{
    ledcWrite(redChannel, red);
    ledcWrite(greenChannel, green);
    ledcWrite(blueChannel, blue);
}

// Get distance from HC-SR04 sensor
float getDistance()
{
    float echoTime;           // Time for pulse to bounce
    float calculatedDistance; // Calculated distance in inches

    // Generate 10µs trigger pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure the length of echo pulse
    echoTime = pulseIn(echoPin, HIGH);

    // Calculate the distance
    calculatedDistance = echoTime / 148.0; // Convert to inches
    // calculatedDistance = echoTime / 58.0; // Uncomment for centimeters

    return calculatedDistance;
}
