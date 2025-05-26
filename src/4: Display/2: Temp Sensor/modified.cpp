/*
  SparkFun Inventor's Kit
  Circuit 4B - Temperature Sensor with Alarm

  The LCD will display readings from a temperature sensor in degrees Celsius and Fahrenheit.
  An alarm will sound when the temperature exceeds 25°C.

  Adapted for ESP32 from original SparkFun Electronics code.
*/

#include <Arduino.h>       // Include the Arduino library
#include <LiquidCrystal.h> // The liquid crystal library contains commands for printing to the display

// Define the pins connected to the LCD for ESP32
// RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 14, 27, 26, 25); // Tell the ESP32 what pins are connected to the display

// Define temperature sensor pin
const int tempSensorPin = 34; // Connect TMP36 sensor to GPIO34 (ADC1_CH6)

// Define buzzer pin
const int buzzerPin = 22; // Connect buzzer to GPIO22

// Temperature threshold for alarm
const float alarmTemperature = 25.0; // Alarm triggers when temperature exceeds 25°C

float voltage = 0;  // The voltage measured from the TMP36
float degreesC = 0; // The temperature in Celsius, calculated from the voltage
float degreesF = 0; // The temperature in Fahrenheit, calculated from the voltage

// Function prototype declaration
void soundAlarm();

void setup()
{
    lcd.begin(16, 2); // Tell the lcd library that we are using a display that is 16x2
    lcd.clear();      // Clear the display

    // Initialize serial communication for debugging
    Serial.begin(9600);
    Serial.println("ESP32 Temperature Sensor with Alarm");

    // Set up the buzzer pin as an output
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW); // Ensure buzzer is off at startup

    // ESP32 ADC setup
    analogSetWidth(12);             // Set ADC resolution to 12 bits (0-4095)
    analogSetAttenuation(ADC_11db); // Set ADC attenuation for full 3.3V range
}

void loop()
{
    // Read the analog value from the temperature sensor
    int sensorValue = analogRead(tempSensorPin);

    // Convert the analog reading (0-4095) to a voltage (0-3.3V)
    voltage = sensorValue * (3.3 / 4095.0);

    // Convert the voltage to a temperature in degrees Celsius
    degreesC = ((voltage - 0.5) * 100.0) + 14;

    // Convert Celsius to Fahrenheit
    degreesF = degreesC * (9.0 / 5.0) + 32.0;

    // Debug output
    Serial.print("Sensor value: ");
    Serial.print(sensorValue);
    Serial.print(", Voltage: ");
    Serial.print(voltage, 2);
    Serial.print("V, Temp: ");
    Serial.print(degreesC, 1);
    Serial.print("°C / ");
    Serial.print(degreesF, 1);
    Serial.println("°F");

    // Display temperature on LCD
    lcd.clear(); // Clear the LCD

    lcd.setCursor(0, 0);      // Set the cursor to the top left position
    lcd.print("Degrees C: "); // Print a label for the data
    lcd.print(degreesC, 1);   // Print the degrees Celsius with 1 decimal place

    lcd.setCursor(0, 1);      // Set the cursor to the lower left position
    lcd.print("Degrees F: "); // Print a label for the data
    lcd.print(degreesF, 1);   // Print the degrees Fahrenheit with 1 decimal place

    // Check if temperature exceeds the alarm threshold
    if (degreesC > alarmTemperature)
    {
        // Temperature is above threshold - trigger alarm
        lcd.setCursor(14, 0); // Set cursor near the end of the top line
        lcd.print("!");       // Display an exclamation mark to indicate alarm
        soundAlarm();         // Sound the alarm
    }
    else
    {
        // Temperature is below threshold - alarm off
        digitalWrite(buzzerPin, LOW); // Make sure buzzer is off
    }

    delay(1000); // Delay for 1 second between readings
}

// Function to sound the alarm
void soundAlarm()
{
    // Beep pattern for the alarm
    digitalWrite(buzzerPin, HIGH);
    delay(300);
    digitalWrite(buzzerPin, LOW);
    delay(200);
    digitalWrite(buzzerPin, HIGH);
    delay(300);
    digitalWrite(buzzerPin, LOW);
}
