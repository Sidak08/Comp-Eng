/*
  ESP32 Servo Control with Potentiometer and 7-Segment Display

  This sketch controls a servo based on potentiometer position
  and shows the current angle on a 7-segment display.
*/

#include <Arduino.h>
#include <ESP32Servo.h>

// Pin assignments
const int potPin = 34;   // GPIO34 for potentiometer (analog input)
const int servoPin = 23; // GPIO23 for servo control

// 7-segment display pins (adjust these based on your wiring)
const int segmentPins[7] = {13, 12, 14, 27, 26, 25, 33}; // a,b,c,d,e,f,g
const int digitPin = 9;                                  // Common pin (for common cathode use HIGH, for common anode use LOW)

// Define the segment patterns for numbers 0-9
// Segments:    a  b  c  d  e  f  g
byte digits[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
};

int potPosition;   // Store potentiometer position
int servoPosition; // Servo position (angle)
int displayValue;  // Value to display (0-9)

bool isCommonCathode = true; // Set to false for common anode display

Servo myservo;

void displayDigit(int num);
void clearDisplay();

void setup()
{
    Serial.begin(115200);
    Serial.println("ESP32 Servo Control with 7-Segment Display");

    // Set up servo
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    myservo.setPeriodHertz(50);
    myservo.attach(servoPin, 500, 2400);

    // Set up 7-segment display pins
    for (int i = 0; i < 7; i++)
    {
        pinMode(segmentPins[i], OUTPUT);
        digitalWrite(segmentPins[i], isCommonCathode ? LOW : HIGH); // Turn off all segments
    }

    // Set common pin
    pinMode(digitPin, OUTPUT);
    digitalWrite(digitPin, isCommonCathode ? HIGH : LOW); // Turn on the digit

    // Display '0' initially
    displayDigit(0);
}

void loop()
{
    // Read potentiometer value
    potPosition = analogRead(potPin);

    // Map to servo range (20-160 degrees)
    servoPosition = map(potPosition, 0, 4095, 20, 160);

    // Move servo
    myservo.write(servoPosition);

    // Calculate display value (divide by 10 to get tens digit: 0-9 for 0-90 degrees)
    displayValue = servoPosition / 20; // This will give 1-8 for the range 20-160
    if (displayValue > 9)
        displayValue = 9; // Keep in single digit range

    // Update display
    displayDigit(displayValue);

    // Debug output
    Serial.print("Pot: ");
    Serial.print(potPosition);
    Serial.print(" | Servo: ");
    Serial.print(servoPosition);
    Serial.print(" | Display: ");
    Serial.println(displayValue);

    delay(100);
}

// Function to display a digit on the 7-segment display
void displayDigit(int num)
{
    if (num < 0 || num > 9)
        return; // Only handle 0-9

    for (int i = 0; i < 7; i++)
    {
        // Set the correct state based on display type
        if (isCommonCathode)
        {
            digitalWrite(segmentPins[i], digits[num][i]);
        }
        else
        {
            digitalWrite(segmentPins[i], !digits[num][i]); // Inverted logic for common anode
        }
    }
}

// Function to clear the display
void clearDisplay()
{
    for (int i = 0; i < 7; i++)
    {
        digitalWrite(segmentPins[i], isCommonCathode ? LOW : HIGH);
    }
}
