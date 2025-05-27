// ESP32 Motion Alarm System with Button Reset and Jerky Servo
// Detects proximity with ultrasonic sensor and activates alarm with
// LED, buzzer, and erratic servo movements until reset by button press

#include <Arduino.h>
#include <ESP32Servo.h> // ESP32 servo library

// Pin definitions
const int trigPin = 26;   // Ultrasonic sensor trigger
const int echoPin = 27;   // Ultrasonic sensor echo
const int redPin = 19;    // RGB LED red pin
const int greenPin = 21;  // RGB LED green pin
const int bluePin = 18;   // RGB LED blue pin
const int buzzerPin = 25; // Buzzer output
const int servoPin = 23;  // Servo control
const int buttonPin = 15; // Reset button input

// PWM settings
const int freq = 5000;       // PWM frequency
const int buzzerChannel = 3; // Buzzer PWM channel
const int redChannel = 0;    // Red PWM channel
const int greenChannel = 1;  // Green PWM channel
const int blueChannel = 2;   // Blue PWM channel
const int resolution = 8;    // 8-bit (0-255)

// Variables
float distance = 0;               // Measured distance
bool alarmActive = false;         // Alarm state
unsigned long alarmStartTime = 0; // Alarm start timestamp
unsigned long lastServoMove = 0;  // Last servo movement timestamp
int servoPosition = 90;           // Servo angle
int jerkCounter = 0;              // Servo jerk pattern counter

Servo myservo;

// Function declarations
float getDistance();
void setColor(int red, int green, int blue);
void triggerAlarm();
void stopAlarm();
bool checkButtonPress();
void servoJerk(); // Function to make the servo jerk

void setup()
{
    Serial.begin(115200);
    Serial.println("ESP32 Motion Alarm System with Jerky Servo");

    // Configure pins
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(buttonPin, INPUT_PULLUP);

    // Initialize PWM channels
    ledcSetup(redChannel, freq, resolution);
    ledcSetup(greenChannel, freq, resolution);
    ledcSetup(blueChannel, freq, resolution);
    ledcSetup(buzzerChannel, 2000, resolution);

    // Connect PWM channels to pins
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
    // Check for alarm reset button
    if (alarmActive && checkButtonPress())
    {
        stopAlarm();
    }

    // Normal monitoring mode
    if (!alarmActive)
    {
        distance = getDistance();

        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" in");

        // Set LED color based on distance
        if (distance <= 10)
        {
            triggerAlarm();
        }
        else if (10 < distance && distance < 20)
        {
            setColor(255, 50, 0); // Yellow for medium distance
        }
        else
        {
            setColor(0, 255, 0);  // Green for far distance
        }
    }
    else
    {
        // Alarm mode
        unsigned long currentMillis = millis();

        // Control servo movement
        servoJerk();

        // Alternating alarm pattern
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

// Core functions

// Create erratic servo movement patterns
void servoJerk()
{
    unsigned long currentMillis = millis();

    // Move at semi-random intervals
    if (currentMillis - lastServoMove >= 50 + random(0, 100))
    {
        jerkCounter++;

        // Different movement patterns
        switch (jerkCounter % 5)
        {
        case 0:
            servoPosition = 20;  // Far left
            break;
        case 1:
            servoPosition = 160; // Far right
            break;
        case 2:
            servoPosition += random(-30, 30); // Small jitter
            break;
        case 3:
            servoPosition = random(20, 160); // Random position
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

// Activate alarm mode
void triggerAlarm()
{
    Serial.println("ALARM TRIGGERED! Press button to reset.");
    alarmActive = true;
    alarmStartTime = millis();
    lastServoMove = millis();
    jerkCounter = 0;
}

// Deactivate alarm and reset system
void stopAlarm()
{
    Serial.println("Alarm stopped. Resuming monitoring.");

    ledcWrite(buzzerChannel, 0);
    myservo.write(90);
    
    setColor(0, 0, 255);
    delay(500);
    setColor(0, 255, 0);

    alarmActive = false;
}

// Check button press with debouncing
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

// Set RGB LED color
void setColor(int red, int green, int blue)
{
    ledcWrite(redChannel, red);
    ledcWrite(greenChannel, green);
    ledcWrite(blueChannel, blue);
}

// Measure distance with ultrasonic sensor
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

    // Get echo response
    echoTime = pulseIn(echoPin, HIGH);

    // Convert to inches
    calculatedDistance = echoTime / 148.0;

    return calculatedDistance;
}
