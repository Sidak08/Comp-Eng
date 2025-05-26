/*
  SparkFun Inventor's Kit
  Circuit 4B - Temperature Sensor

  The LCD will display readings from a temperature sensor in degrees Celsius and Fahrenheit.

  Adapted for ESP32 from original SparkFun Electronics code.
*/

#include <Arduino.h>       // Include the Arduino library
#include <LiquidCrystal.h> // The liquid crystal library contains commands for printing to the display

// Define the pins connected to the LCD for ESP32
// RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 14, 27, 26, 25); // Tell the ESP32 what pins are connected to the display

// Define temperature sensor pin
const int tempSensorPin = 34; // Connect TMP36 sensor to GPIO34 (ADC1_CH6)

float voltage = 0;  // The voltage measured from the TMP36
float degreesC = 0; // The temperature in Celsius, calculated from the voltage
float degreesF = 0; // The temperature in Fahrenheit, calculated from the voltage

void setup()
{
    lcd.begin(16, 2); // Tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
    lcd.clear();      // Clear the display

    // Initialize serial communication for debugging
    Serial.begin(9600);
    Serial.println("ESP32 Temperature Sensor");

    // ESP32 ADC setup (not required but good practice)
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
    // For TMP36: Temp in °C = (Voltage - 0.5) * 100
    degreesC = ((voltage - 0.5) * 100.0) + 12;

    // Convert Celsius to Fahrenheit
    degreesF = degreesC * (9.0 / 5.0) + 32.0;

    // Print values to serial monitor for debugging
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

    delay(1000); // Delay for 1 second between each reading (this makes the display less noisy)
}
