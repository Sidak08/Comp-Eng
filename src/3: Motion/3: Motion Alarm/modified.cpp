/*
  ESP32 Motion Alarm System with Button Reset and Jerky Servo

  Control the color of an RGB LED using an ultrasonic distance sensor.
  When an object is close to the sensor, the servo jerks erratically,
  the buzzer sounds, and the LED turns red. Alarm continues until
  reset by button press.

  Adapted from SparkFun Inventor's Kit for ESP32.
*/

#include <Arduino.h>
#include <ESP32Servo.h> // ESP32 servo library

// Pin definitions - keeping original ultrasonic and RGB LED pins
const int trigPin = 26;   // Trigger pin for HC-SR04
const int echoPin = 27;   // Echo pin for HC-SR04
const int redPin = 19;    // Controls red LED
const int greenPin = 21;  // Controls green LED
const int bluePin = 18;   // Controls blue LED
const int buzzerPin = 25; // Pin for buzzer
const int servoPin = 23;  // Pin for servo control
const int buttonPin = 15; // Pin for reset button

// PWM properties for RGB LED and buzzer
const int freq = 5000;       // 5000 Hz PWM frequency
const int buzzerChannel = 3; // PWM channel for buzzer
const int redChannel = 0;    // PWM channel for red
const int greenChannel = 1;  // PWM channel for green
const int blueChannel = 2;   // PWM channel for blue
const int resolution = 8;    // 8-bit resolution (0-255)

float distance = 0;               // Stores the distance measured by the distance sensor
bool alarmActive = false;         // Flag to track if alarm is currently active
unsigned long alarmStartTime = 0; // When the alarm was triggered
unsigned long lastServoMove = 0;  // Timing for servo jerks
int servoPosition = 90;           // Current servo position
int jerkCounter = 0;              // For creating jerky servo patterns

Servo myservo; // Create a servo object

// Function declarations
float getDistance();
void setColor(int red, int green, int blue);
void triggerAlarm();
void stopAlarm();
bool checkButtonPress();
void servoJerk(); // Function to make the servo jerk

void setup()
{
    Serial.begin(115200); // Higher baud rate for ESP32
    Serial.println("ESP32 Motion Alarm System with Jerky Servo");

    pinMode(trigPin, OUTPUT);         // Trigger pin outputs pulses
    pinMode(echoPin, INPUT);          // Echo pin measures returning pulses
    pinMode(buttonPin, INPUT_PULLUP); // Button pin with internal pull-up resistor

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
    myservo.setPeriodHertz(50);          // Standard 50Hz servo
    myservo.attach(servoPin, 500, 2400); // Attaches the servo with wider pulse range for better compatibility

    // Center the servo initially
    myservo.write(90);
    delay(500);

    // Indicate system is ready
    setColor(0, 0, 255); // Blue light indicates system is ready
    delay(1000);
    setColor(0, 255, 0); // Green light indicates monitoring mode
}

void loop()
{
    // Always check if the button is pressed to stop the alarm
    if (alarmActive && checkButtonPress())
    {
        stopAlarm();
    }

    // If alarm is not active, monitor the distance
    if (!alarmActive)
    {
        distance = getDistance(); // Get distance from sensor

        Serial.print("Distance: ");
        Serial.print(distance); // Print the distance
        Serial.println(" in");  // Print units

        if (distance <= 10)
        {                   // If object is close
            triggerAlarm(); // Trigger the alarm
        }
        else if (10 < distance && distance < 20)
        { // If object is at medium distance
            // Make the RGB LED yellow
            setColor(255, 50, 0);
        }
        else
        { // If object is far away
            // Make the RGB LED green
            setColor(0, 255, 0);
        }
    }
    else
    {
        // Alarm is active - continue the alarm sequence with jerky servo movement
        unsigned long currentMillis = millis();

        // Make the servo jerk in erratic patterns
        servoJerk();

        // Alternating buzzer tones
        if ((currentMillis - alarmStartTime) % 300 < 150)
        {
            ledcWriteTone(buzzerChannel, 800); // Higher pitch alarm
            setColor(255, 0, 0);               // Bright red
        }
        else
        {
            ledcWriteTone(buzzerChannel, 400); // Lower pitch alarm
            setColor(200, 0, 0);               // Slightly dimmer red
        }
    }

    delay(10); // Very short delay for more responsive servo jerking
}

//------------------FUNCTIONS-------------------------------

// Make the servo jerk erratically
void servoJerk()
{
    unsigned long currentMillis = millis();

    // Only jerk the servo at specific intervals
    if (currentMillis - lastServoMove >= 50 + random(0, 100))
    { // Semi-random timing
        jerkCounter++;

        // Create different jerking patterns
        switch (jerkCounter % 5)
        {
        case 0: // Quick jerk to one extreme
            servoPosition = 20;
            break;
        case 1: // Quick jerk to other extreme
            servoPosition = 160;
            break;
        case 2: // Small jitter
            servoPosition += random(-30, 30);
            break;
        case 3: // Random position
            servoPosition = random(20, 160);
            break;
        case 4: // Return to center briefly
            servoPosition = 90;
            break;
        }

        // Keep servo position in safe range
        servoPosition = constrain(servoPosition, 20, 160);

        // Move the servo
        myservo.write(servoPosition);
        lastServoMove = currentMillis;
    }
}

// Trigger the alarm sequence
void triggerAlarm()
{
    Serial.println("ALARM TRIGGERED! Press button to reset.");
    alarmActive = true;
    alarmStartTime = millis();
    lastServoMove = millis();
    jerkCounter = 0;
}

// Stop the alarm and reset the system
void stopAlarm()
{
    Serial.println("Alarm stopped. Resuming monitoring.");

    // Stop the buzzer and center the servo
    ledcWrite(buzzerChannel, 0); // Turn buzzer off
    myservo.write(90);           // Center the servo

    // Visual feedback that alarm is cleared
    setColor(0, 0, 255); // Blue light
    delay(500);          // Brief pause
    setColor(0, 255, 0); // Back to green monitoring state

    alarmActive = false; // Reset alarm flag
}

// Check if the button is pressed (with debounce)
bool checkButtonPress()
{
    static unsigned long lastDebounceTime = 0;
    static bool lastButtonState = HIGH;
    bool buttonState = digitalRead(buttonPin);

    // If the button state changed, reset debounce timer
    if (buttonState != lastButtonState)
    {
        lastDebounceTime = millis();
    }

    // If the button state has been stable for 50ms and is LOW (pressed)
    if ((millis() - lastDebounceTime) > 50 && buttonState == LOW)
    {
        lastButtonState = buttonState;
        return true;
    }

    lastButtonState = buttonState;
    return false;
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
