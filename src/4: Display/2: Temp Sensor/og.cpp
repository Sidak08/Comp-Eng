// ESP32 Temperature Sensor Display
// Reads temperature from TMP36 sensor and displays in Celsius and Fahrenheit

#include <Arduino.h>
#include <LiquidCrystal.h> // Library for controlling LCD display

// LCD pin connections (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

// Temperature sensor pin
const int tempSensorPin = 34; // TMP36 sensor on GPIO34

float voltage = 0;  // Sensor voltage
float degreesC = 0; // Temperature in Celsius
float degreesF = 0; // Temperature in Fahrenheit

void setup()
{
    lcd.begin(16, 2);
    lcd.clear();

    Serial.begin(9600);
    Serial.println("ESP32 Temperature Sensor");

    // Configure ADC
    analogSetWidth(12);             // 12-bit resolution (0-4095)
    analogSetAttenuation(ADC_11db); // Full 3.3V range
}

void loop()
{
    // Read temperature sensor
    int sensorValue = analogRead(tempSensorPin);
    
    // Convert reading to voltage
    voltage = sensorValue * (3.3 / 4095.0);
    
    // Calculate temperature values
    degreesC = ((voltage - 0.5) * 100.0) + 12;
    degreesF = degreesC * (9.0 / 5.0) + 32.0;

    // Output values to serial
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
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Degrees C: ");
    lcd.print(degreesC, 1);

    lcd.setCursor(0, 1);
    lcd.print("Degrees F: ");
    lcd.print(degreesF, 1);

    delay(1000); // Update interval
}
