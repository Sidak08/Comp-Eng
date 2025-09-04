#include <Arduino.h>
#include <ESP32Servo.h>

const int trigPin = 26;
const int echoPin = 27;
const int redPin = 19;
const int greenPin = 21;
const int bluePin = 18;
const int buzzerPin = 25;
const int servoPin = 23;
const int buttonPin = 15;

// PWM settings
const int freq = 5000;
const int buzzerChannel = 3;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

float distance = 0;
bool alarmActive = false;
unsigned long alarmStartTime = 0;
unsigned long lastServoMove = 0;
int servoPosition = 90;
int jerkCounter = 0;

Servo myservo;
float getDistance();
void setColor(int red, int green, int blue);
void triggerAlarm();
void stopAlarm();
bool checkButtonPress();
void servoJerk();

void setup()
{
    Serial.begin(115200);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(buttonPin, INPUT_PULLUP);

    ledcSetup(redChannel, freq, resolution);
    ledcSetup(greenChannel, freq, resolution);
    ledcSetup(blueChannel, freq, resolution);
    ledcSetup(buzzerChannel, 2000, resolution);

    ledcAttachPin(redPin, redChannel);
    ledcAttachPin(greenPin, greenChannel);
    ledcAttachPin(bluePin, blueChannel);
    ledcAttachPin(buzzerPin, buzzerChannel);

    // Initialize servo
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    myservo.setPeriodHertz(50);
    myservo.attach(servoPin, 500, 2400);

    // Center servo
    myservo.write(90);
    delay(500);

    // Startup indication
    setColor(0, 0, 255); // Blue for startup
    delay(1000);
    setColor(0, 255, 0); // Green for monitoring
}

void loop()
{
    if (alarmActive && checkButtonPress())
    {
        stopAlarm();
    }

    if (!alarmActive)
    {
        distance = getDistance();
        if (distance <= 10)
        {
            triggerAlarm();
        }
        else if (10 < distance && distance < 20)
        {
            setColor(255, 50, 0);
        }
        else
        {
            setColor(0, 255, 0);
        }
    }
    else
    {
        unsigned long currentMillis = millis();
        servoJerk();

        if ((currentMillis - alarmStartTime) % 300 < 150)
        {
            ledcWriteTone(buzzerChannel, 800);
            setColor(255, 0, 0);
        }
        else
        {
            ledcWriteTone(buzzerChannel, 400);
            setColor(200, 0, 0);
        }
    }

    delay(10);
}

void servoJerk()
{
    unsigned long currentMillis = millis();

    if (currentMillis - lastServoMove >= 50 + random(0, 100))
    {
        jerkCounter++;

        switch (jerkCounter % 5)
        {
        case 0:
            servoPosition = 20;  // Far left
            break;
        case 1:
            servoPosition = 160; // Far right
            break;
        case 2:
            servoPosition += random(-30, 30);
            break;
        case 3:
            servoPosition = random(20, 160);
            break;
        case 4:
            servoPosition = 90; // Center
            break;
        }

        servoPosition = constrain(servoPosition, 20, 160);
        myservo.write(servoPosition);
        lastServoMove = currentMillis;
    }
}

void triggerAlarm()
{
    alarmActive = true;
    alarmStartTime = millis();
    lastServoMove = millis();
    jerkCounter = 0;
}

void stopAlarm()
{

    ledcWrite(buzzerChannel, 0);
    myservo.write(90);

    setColor(0, 0, 255);
    delay(500);
    setColor(0, 255, 0);

    alarmActive = false;
}

bool checkButtonPress()
{
    static unsigned long lastDebounceTime = 0;
    static bool lastButtonState = HIGH;
    bool buttonState = digitalRead(buttonPin);

    if (buttonState != lastButtonState)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > 50 && buttonState == LOW)
    {
        lastButtonState = buttonState;
        return true;
    }

    lastButtonState = buttonState;
    return false;
}

void setColor(int red, int green, int blue)
{
    ledcWrite(redChannel, red);
    ledcWrite(greenChannel, green);
    ledcWrite(blueChannel, blue);
}

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

    echoTime = pulseIn(echoPin, HIGH);

    calculatedDistance = echoTime / 148.0;

    return calculatedDistance;
}
