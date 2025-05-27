/*
  ESP32 Distance Sensor with RGB LED and Adjustable Thresholds

  Control the color of an RGB LED using an ultrasonic distance sensor.
  A potentiometer adjusts the distance thresholds for color changes.

  Adapted from SparkFun Inventor's Kit for ESP32.
*/

#include <Arduino.h>

// Pin definitions for ESP32
const int trigPin = 26; // Trigger pin for HC-SR04
const int echoPin = 27; // Echo pin for HC-SR04
const int potPin = 34;  // Analog pin for potentiometer (GPIO34 - doesn't conflict with RGB pins)

const int redPin = 19;   // Controls red LED
const int greenPin = 21; // Controls green LED
const int bluePin = 18;  // Controls blue LED

// PWM properties
const int freq = 5000;      // 5000 Hz PWM frequency
const int redChannel = 0;   // PWM channel for red
const int greenChannel = 1; // PWM channel for green
const int blueChannel = 2;  // PWM channel for blue
const int resolution = 8;   // 8-bit resolution (0-255)

float distance = 0;   // Stores the distance measured by the distance sensor
int potValue = 0;     // Stores the potentiometer value
int threshold1 = 10;  // First color change threshold (default)
int threshold2 = 20;  // Second color change threshold (default)
int maxDistance = 60; // Maximum measurable distance for scaling

// Function declarations - these must appear before they're called
float getDistance();
void setColor(int red, int green, int blue);
void updateThresholds();

void setup()
{
    Serial.begin(115200); // Higher baud rate for ESP32
    Serial.println("ESP32 Ultrasonic Distance Sensor with Adjustable Thresholds");

    pinMode(trigPin, OUTPUT); // Trigger pin outputs pulses
    pinMode(echoPin, INPUT);  // Echo pin measures returning pulses
    pinMode(potPin, INPUT);   // Potentiometer input

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
    // Read the potentiometer and update thresholds
    updateThresholds();

    // Get distance from sensor
    distance = getDistance();

    // Print values for debugging
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print(" in | Threshold1: ");
    Serial.print(threshold1);
    Serial.print(" | Threshold2: ");
    Serial.println(threshold2);

    // Change LED color based on distance and adjustable thresholds
    if (distance <= threshold1)
    { // Object is close
        // Make the RGB LED red
        setColor(255, 0, 0);
    }
    else if (distance > threshold1 && distance < threshold2)
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

// Update thresholds based on potentiometer value
void updateThresholds()
{
    // Read the potentiometer (0-4095 on ESP32)
    potValue = analogRead(potPin);

    // Map the potentiometer value to a reasonable range for threshold1 (1-30 inches)
    threshold1 = map(potValue, 0, 4095, 1, 30);

    // Set threshold2 to be threshold1 + 10 inches
    threshold2 = threshold1 + 10;

    // Make sure threshold2 doesn't exceed maximum distance
    if (threshold2 > maxDistance)
    {
        threshold2 = maxDistance;
    }
}

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
