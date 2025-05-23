/*
  ESP32 Motion Alarm System
  
  Control the color of an RGB LED using an ultrasonic distance sensor.
  When an object is close to the sensor, buzz the buzzer and wiggle the servo motor.
  
  Adapted from SparkFun Inventor's Kit for ESP32.
*/

#include <Arduino.h>
#include <ESP32Servo.h>  // ESP32 servo library

// Pin definitions - keeping original ultrasonic and RGB LED pins
const int trigPin = 26;     // Trigger pin for HC-SR04
const int echoPin = 27;     // Echo pin for HC-SR04
const int redPin = 19;      // Controls red LED
const int greenPin = 21;    // Controls green LED
const int bluePin = 18;     // Controls blue LED
const int buzzerPin = 25;   // Pin for buzzer
const int servoPin = 23;    // Pin for servo control

// PWM properties for RGB LED and buzzer
const int freq = 5000;      // 5000 Hz PWM frequency
const int buzzerChannel = 3;// PWM channel for buzzer
const int redChannel = 0;   // PWM channel for red
const int greenChannel = 1; // PWM channel for green
const int blueChannel = 2;  // PWM channel for blue
const int resolution = 8;   // 8-bit resolution (0-255)

float distance = 0;         // Stores the distance measured by the distance sensor

Servo myservo;              // Create a servo object

// Function declarations
float getDistance();
void setColor(int red, int green, int blue);

void setup()
{
  Serial.begin(115200);     // Higher baud rate for ESP32
  Serial.println("ESP32 Motion Alarm System");

  pinMode(trigPin, OUTPUT); // Trigger pin outputs pulses
  pinMode(echoPin, INPUT);  // Echo pin measures returning pulses
  
  // Configure PWM for RGB LED
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);
  ledcSetup(buzzerChannel, 2000, resolution); // Buzzer with different frequency
  
  // Attach the PWM channels to the GPIO pins
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  ledcAttachPin(buzzerPin, buzzerChannel);
  
  // Set up servo motor
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50); // Standard 50Hz servo
  myservo.attach(servoPin, 500, 2400); // Attaches the servo with wider pulse range for better compatibility
  
  // Center the servo initially
  myservo.write(90);
  delay(500);
}

void loop() {
  distance = getDistance();  // Get distance from sensor

  Serial.print("Distance: ");
  Serial.print(distance);    // Print the distance
  Serial.println(" in");     // Print units

  if (distance <= 10) {      // If object is close
    // Make the RGB LED red
    setColor(255, 0, 0);

    // Wiggle servo and sound buzzer
    ledcWriteTone(buzzerChannel, 272);  // Buzz at 272Hz
    myservo.write(10);                  // Move servo to 10 degrees
    delay(100);                         // Wait 100ms
    
    ledcWrite(buzzerChannel, 0);        // Turn buzzer off
    myservo.write(150);                 // Move servo to 150 degrees
    delay(100);                         // Wait 100ms
  } 
  else if (10 < distance && distance < 20) {  // If object is at medium distance
    // Make the RGB LED yellow
    setColor(255, 50, 0);
  } 
  else {                                // If object is far away
    // Make the RGB LED green
    setColor(0, 255, 0);
  }

  delay(50);  // Delay between readings
}

//------------------FUNCTIONS-------------------------------

// Set the RGB LED color
void setColor(int red, int green, int blue) {
  ledcWrite(redChannel, red);
  ledcWrite(greenChannel, green);
  ledcWrite(blueChannel, blue);
}

// Get distance from HC-SR04 sensor
float getDistance() {
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
