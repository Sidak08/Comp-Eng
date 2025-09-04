#include <Arduino.h>
#include <ESP32Servo.h>


const int potPin = 34;
const int servoPin = 23;

// 7-segment display pins
const int segmentPins[7] = {13, 12, 14, 27, 26, 25, 33}; // a,b,c,d,e,f,g
const int digitPin = 9;

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

int potPosition;
int servoPosition;
int displayValue;

bool isCommonCathode = true;

Servo myservo;

void displayDigit(int num);
void clearDisplay();

void setup()
{
    Serial.begin(115200);

    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    myservo.setPeriodHertz(50);
    myservo.attach(servoPin, 500, 2400);

    // Initialize display segments
    for (int i = 0; i < 7; i++)
    {
        pinMode(segmentPins[i], OUTPUT);
        digitalWrite(segmentPins[i], isCommonCathode ? LOW : HIGH);
    }

    // Initialize common pin
    pinMode(digitPin, OUTPUT);
    digitalWrite(digitPin, isCommonCathode ? HIGH : LOW);

    // Display '0' initially
    displayDigit(0);
}

void loop()
{
    potPosition = analogRead(potPin);
    servoPosition = map(potPosition, 0, 4095, 20, 160);
    myservo.write(servoPosition);

    displayValue = servoPosition / 20;
    if (displayValue > 9)
        displayValue = 9;

    displayDigit(displayValue);

    delay(100);
    delay(100);
}

void displayDigit(int num)
{
    if (num < 0 || num > 9)
        return;

    for (int i = 0; i < 7; i++)
    {
        digitalWrite(segmentPins[i], isCommonCathode ?
                    digits[num][i] : !digits[num][i]);
    }
}

// Clear the display
void clearDisplay()
{
    for (int i = 0; i < 7; i++)
    {
        digitalWrite(segmentPins[i], isCommonCathode ? LOW : HIGH);
    }
}
