// ESP32 Distance Sensor with RGB LED
// Controls RGB LED color based on measured distance from ultrasonic sensor

#include <Arduino.h>

// Pin definitions
const int trigPin = 26; // Ultrasonic sensor trigger
const int echoPin = 27; // Ultrasonic sensor echo

const int redPin = 19;   // RGB LED red pin
const int greenPin = 21; // RGB LED green pin
const int bluePin = 18;  // RGB LED blue pin

// PWM settings
const int freq = 5000;      // PWM frequency
const int redChannel = 0;   // Red PWM channel
const int greenChannel = 1; // Green PWM channel
const int blueChannel = 2;  // Blue PWM channel
const int resolution = 8;   // 8-bit (0-255)

float distance = 0; // Measured distance

// Function prototypes
float getDistance();
void setColor(int red, int green, int blue);

void setup()
{
    Serial.begin(115200);
    Serial.println("ESP32 Ultrasonic Distance Sensor with RGB LED");

    pinMode(trigPin, OUTPUT); // Configure trigger as output
    pinMode(echoPin, INPUT);  // Configure echo as input

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
    distance = getDistance();

    Serial.print(distance);
    Serial.println(" in");

    // Set LED color based on distance
    if (distance <= 10) {
        setColor(255, 0, 0);    // Red for close objects
    }
    else if (10 < distance && distance < 20) {
        setColor(255, 50, 0);   // Yellow for medium distance
    }
    else {
        setColor(0, 255, 0);    // Green for distant objects
    }

    delay(50);
}

// Core functions

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
