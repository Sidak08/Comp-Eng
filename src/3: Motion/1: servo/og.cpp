// ESP32 Servo Control with Potentiometer
// Controls a servo motor position based on potentiometer input
#include <Arduino.h>
#include <ESP32Servo.h> // ESP32 servo library

// Pin definitions
const int potPin = 34;   // Potentiometer input
const int servoPin = 23; // Servo control

int potPosition;   // Potentiometer reading
int servoPosition; // Servo angle (degrees)

Servo myservo;

void setup()
{
    Serial.begin(115200);
    Serial.println("ESP32 Servo Control with Potentiometer");

    // Initialize servo timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    myservo.setPeriodHertz(50);
    myservo.attach(servoPin, 500, 2400);
}

void loop()
{
    // Read potentiometer and map to servo position
    potPosition = analogRead(potPin);
    servoPosition = map(potPosition, 0, 4095, 20, 160);
    
    // Move servo to calculated position
    myservo.write(servoPosition);

    // Output values to serial
    Serial.print("Pot: ");
    Serial.print(potPosition);
    Serial.print(" Servo: ");
    Serial.println(servoPosition);

    delay(15);
}
