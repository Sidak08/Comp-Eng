/*
  SparkFun Inventor's Kit - Modified for ESP32
  Circuit 3A-Servo

  Move a servo based on potentiometer position.

  This sketch was adapted for ESP32 from the original SparkFun code.
*/
#include <Arduino.h>
#include <ESP32Servo.h> // ESP32 servo library

// ESP32 pin assignments
const int potPin = 34;   // GPIO34 for potentiometer (analog input)
const int servoPin = 23; // GPIO23 for servo control

int potPosition;   // this variable will store the position of the potentiometer
int servoPosition; // the servo will move to this position

Servo myservo; // create a servo object

void setup()
{
    Serial.begin(115200); // Initialize serial for debugging
    Serial.println("ESP32 Servo Control with Potentiometer");

    // ESP32Servo library supports up to 16 servos
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    // Standard 50Hz servo
    myservo.setPeriodHertz(50);

    // Attach the servo to the specified pin
    myservo.attach(servoPin, 500, 2400); // Use wider pulse width range for better compatibility
}

void loop()
{
    potPosition = analogRead(potPin); // ESP32 has 12-bit ADC (0-4095)

    // Map potentiometer value to servo angle (avoiding extremes)
    servoPosition = map(potPosition, 0, 4095, 20, 160);

    myservo.write(servoPosition);

    // Print values for debugging
    Serial.print("Pot: ");
    Serial.print(potPosition);
    Serial.print(" Servo: ");
    Serial.println(servoPosition);

    delay(15); // Small delay for stability
}
