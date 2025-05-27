#include <Arduino.h>

// Pins
const int photoresistorPin = 32;
const int potentiometerPin = 34;
const int RedPin = 22;
const int GreenPin = 23;
const int BluePin = 21;
const int signalPin = 5;     // HIGH when LED is off

// Threshold for the photoresistor
const int threshold = 3500;

// PWM settings
const int pwmFreq = 5000;    
const int pwmResolution = 8; // 0-255
const int RedChannel = 0;
const int GreenChannel = 1;
const int BlueChannel = 2;

// Function declaration
void turnOff();

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting setup...");

    // Configure LED pins as PWM outputs
    ledcSetup(RedChannel, pwmFreq, pwmResolution);
    ledcAttachPin(RedPin, RedChannel);
    Serial.println("Red LED configured.");

    ledcSetup(GreenChannel, pwmFreq, pwmResolution);
    ledcAttachPin(GreenPin, GreenChannel);
    Serial.println("Green LED configured.");

    ledcSetup(BlueChannel, pwmFreq, pwmResolution);
    ledcAttachPin(BluePin, BlueChannel);
    Serial.println("Blue LED configured.");

    // Configure signal pin
    pinMode(signalPin, OUTPUT);
    digitalWrite(signalPin, LOW);
    Serial.println("Signal pin configured.");

    Serial.println("Setup complete.");
}

void loop()
{
    // Read sensors
    int photoresistor = analogRead(photoresistorPin);
    int potentiometer = analogRead(potentiometerPin);

    Serial.print("Photoresistor value: ");
    Serial.print(photoresistor);
    Serial.print("  Potentiometer value: ");
    Serial.println(potentiometer);

    if (photoresistor > threshold)
    {
        // Dark environment - turn on LED
        Serial.println("It's dark. Turning on the LED...");
        digitalWrite(signalPin, LOW);

        // Map potentiometer to color spectrum
        int redValue = map(potentiometer, 0, 4095, 255, 0);
        int greenValue = map(potentiometer, 0, 4095, 0, 255);
        int blueValue = map(potentiometer, 2048, 4095, 0, 255);

        if (potentiometer < 2048)
        {
            blueValue = 0;
        }

        Serial.print("RGB Values - Red: ");
        Serial.print(redValue);
        Serial.print(", Green: ");
        Serial.print(greenValue);
        Serial.print(", Blue: ");
        Serial.println(blueValue);

        // Set LED colors
        ledcWrite(RedChannel, redValue);
        ledcWrite(GreenChannel, greenValue);
        ledcWrite(BlueChannel, blueValue);
    }
    else
    {
        // Bright environment - turn off LED
        Serial.println("It's bright. Turning off the LED...");
        turnOff();
        digitalWrite(signalPin, HIGH);
    }

    delay(100);
}

void turnOff()
{
    ledcWrite(RedChannel, 0);
    ledcWrite(GreenChannel, 0);
    ledcWrite(BlueChannel, 0);
}
