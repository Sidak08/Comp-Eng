// ESP32 Distance Sensor with RGB LED and Adjustable Thresholds
// Controls RGB LED color based on distance with adjustable thresholds

#include <Arduino.h>

// Pin definitions
const int trigPin = 26;  // Ultrasonic sensor trigger
const int echoPin = 27;  // Ultrasonic sensor echo
const int potPin = 34;   // Potentiometer input

const int redPin = 19;   // RGB LED red pin
const int greenPin = 21; // RGB LED green pin
const int bluePin = 18;  // RGB LED blue pin

// PWM settings
const int freq = 5000;      // PWM frequency
const int redChannel = 0;   // Red PWM channel
const int greenChannel = 1; // Green PWM channel
const int blueChannel = 2;  // Blue PWM channel
const int resolution = 8;   // 8-bit (0-255)

// Variables
float distance = 0;   // Measured distance
int potValue = 0;     // Potentiometer reading
int threshold1 = 10;  // First threshold
int threshold2 = 20;  // Second threshold
int maxDistance = 60; // Maximum measurable distance

// Function prototypes
float getDistance();
void setColor(int red, int green, int blue);
void updateThresholds();

void setup()
{
    Serial.begin(115200);
    Serial.println("ESP32 Ultrasonic Distance Sensor with Adjustable Thresholds");

    pinMode(trigPin, OUTPUT); // Configure trigger as output
    pinMode(echoPin, INPUT);  // Configure echo as input
    pinMode(potPin, INPUT);   // Configure potentiometer as input

    // Initialize PWM for RGB LED
    ledcSetup(redChannel, freq, resolution);
    ledcSetup(greenChannel, freq, resolution);
    ledcSetup(blueChannel, freq, resolution);

    // Connect PWM channels to pins
    ledcAttachPin(redPin, redChannel);
    ledcAttachPin(greenPin, greenChannel);
    ledcAttachPin(bluePin, blueChannel);
}

void loop()
{
    updateThresholds();
    distance = getDistance();

    // Output measurements
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print(" in | Threshold1: ");
    Serial.print(threshold1);
    Serial.print(" | Threshold2: ");
    Serial.println(threshold2);

    // Set LED color based on distance
    if (distance <= threshold1) {
        setColor(255, 0, 0);    // Red for close objects
    }
    else if (distance > threshold1 && distance < threshold2) {
        setColor(255, 50, 0);   // Yellow for medium distance
    }
    else {
        setColor(0, 255, 0);    // Green for distant objects
    }

    delay(50);
}

// Core functions

// Update distance thresholds based on potentiometer reading
void updateThresholds()
{
    potValue = analogRead(potPin);
    
    // Map potentiometer to threshold range
    threshold1 = map(potValue, 0, 4095, 1, 30);
    
    // Set second threshold relative to first
    threshold2 = threshold1 + 10;
    
    // Constrain to maximum distance
    if (threshold2 > maxDistance)
    {
        threshold2 = maxDistance;
    }
}

// Set RGB LED color values
void setColor(int red, int green, int blue)
{
    ledcWrite(redChannel, red);
    ledcWrite(greenChannel, green);
    ledcWrite(blueChannel, blue);
}

// Measure distance using ultrasonic sensor
float getDistance()
{
    float echoTime;
    float calculatedDistance;

    // Send trigger pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Get echo response time
    echoTime = pulseIn(echoPin, HIGH);

    // Convert time to distance (inches)
    calculatedDistance = echoTime / 148.0;

    return calculatedDistance;
}
