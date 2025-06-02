#include <Arduino.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);
const int tempSensorPin = 34;

float voltage = 0;
float degreesC = 0;
float degreesF = 0;

void setup()
{
    lcd.begin(16, 2);
    lcd.clear();

    Serial.begin(9600);

    // Configure ADC
    analogSetWidth(12);
    analogSetAttenuation(ADC_11db);
}

void loop()
{
    int sensorValue = analogRead(tempSensorPin);
    voltage = sensorValue * (3.3 / 4095.0);

    degreesC = ((voltage - 0.5) * 100.0) + 12;
    degreesF = degreesC * (9.0 / 5.0) + 32.0;

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Degrees C: ");
    lcd.print(degreesC, 1);

    lcd.setCursor(0, 1);
    lcd.print("Degrees F: ");
    lcd.print(degreesF, 1);

    delay(1000);
}
