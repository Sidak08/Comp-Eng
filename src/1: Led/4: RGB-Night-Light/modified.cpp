#include <Arduino.h>

// Define the analog input pins
const int photoresistorPin = 32; // Photoresistor connected to GPIO32
const int potentiometerPin = 34; // Potentiometer connected to GPIO34

// Define the LED pins
const int RedPin = 22;
const int GreenPin = 23;
const int BluePin = 21;

// Define the signal pin
const int signalPin = 5; // Pin 5 will be HIGH when the LED is off

// Threshold for the photoresistor
const int threshold = 3500;

// PWM channel and resolution settings
const int pwmFreq = 5000;    // PWM frequency
const int pwmResolution = 8; // 8-bit resolution (0-255)
const int RedChannel = 0;    // PWM channel for Red LED
const int GreenChannel = 1;  // PWM channel for Green LED
const int BlueChannel = 2;   // PWM channel for Blue LED

// Forward declaration for turnOff()
void turnOff();

void setup()
{
    Serial.begin(9600); // Start a serial connection
    Serial.println("Starting setup...");

    // Configure the LED pins as PWM outputs
    ledcSetup(RedChannel, pwmFreq, pwmResolution);
    ledcAttachPin(RedPin, RedChannel);
    Serial.println("Red LED configured.");

    ledcSetup(GreenChannel, pwmFreq, pwmResolution);
    ledcAttachPin(GreenPin, GreenChannel);
    Serial.println("Green LED configured.");

    ledcSetup(BlueChannel, pwmFreq, pwmResolution);
    ledcAttachPin(BluePin, BlueChannel);
    Serial.println("Blue LED configured.");

    // Configure the signal pin as an output
    pinMode(signalPin, OUTPUT);
    digitalWrite(signalPin, LOW); // Start with the signal pin LOW
    Serial.println("Signal pin configured.");

    Serial.println("Setup complete.");
}

void loop()
{
    // Read the photoresistor and potentiometer values
    int photoresistor = analogRead(photoresistorPin);
    int potentiometer = analogRead(potentiometerPin);

    // Log the sensor values
    Serial.print("Photoresistor value: ");
    Serial.print(photoresistor);
    Serial.print("  Potentiometer value: ");
    Serial.println(potentiometer);

    if (photoresistor > threshold)
    {
        // If it's dark, turn on the LED
        Serial.println("It's dark. Turning on the LED...");
        digitalWrite(signalPin, LOW); // Turn off the signal pin

        // Map the potentiometer value (0–4095) to a color spectrum
        int redValue = map(potentiometer, 0, 4095, 255, 0);     // Red decreases
        int greenValue = map(potentiometer, 0, 4095, 0, 255);   // Green increases
        int blueValue = map(potentiometer, 2048, 4095, 0, 255); // Blue increases in the second half

        // Clamp blueValue to 0 if potentiometer is in the first half
        if (potentiometer < 2048)
        {
            blueValue = 0;
        }

        // Log the RGB values
        Serial.print("RGB Values - Red: ");
        Serial.print(redValue);
        Serial.print(", Green: ");
        Serial.print(greenValue);
        Serial.print(", Blue: ");
        Serial.println(blueValue);

        // Set the RGB LED colors
        ledcWrite(RedChannel, redValue);
        ledcWrite(GreenChannel, greenValue);
        ledcWrite(BlueChannel, blueValue);
    }
    else
    {
        // If it's bright, turn off the LED
        Serial.println("It's bright. Turning off the LED...");
        turnOff();
        digitalWrite(signalPin, HIGH); // Turn on the signal pin
    }

    delay(100); // Short delay for readability
}

// Function to turn off the LED
void turnOff()
{
    ledcWrite(RedChannel, 0);
    ledcWrite(GreenChannel, 0);
    ledcWrite(BlueChannel, 0);
}
