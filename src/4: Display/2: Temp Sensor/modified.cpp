#include <Arduino.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

const int tempSensorPin = 34;
const int buzzerPin = 22;

const float alarmTemperature = 25.0;

float voltage = 0;
float degreesC = 0;
float degreesF = 0;

void soundAlarm();

void setup()
{
    lcd.begin(16, 2);
    lcd.clear();

    Serial.begin(9600);

    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);

    analogSetWidth(12);
    analogSetAttenuation(ADC_11db);
}

void loop()
{
    int sensorValue = analogRead(tempSensorPin);

    voltage = sensorValue * (3.3 / 4095.0);

    // Calculate temperature values
    degreesC = ((voltage - 0.5) * 100.0) + 14;
    degreesF = degreesC * (9.0 / 5.0) + 32.0;

    // Display temperature on LCD
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Degrees C: ");
    lcd.print(degreesC, 1);

    lcd.setCursor(0, 1);
    lcd.print("Degrees F: ");
    lcd.print(degreesF, 1);

    if (degreesC > alarmTemperature)
    {
        lcd.setCursor(14, 0);
        lcd.print("!");
        soundAlarm();
    }
    else
    {
        digitalWrite(buzzerPin, LOW);
    }

    delay(1000);
}

void soundAlarm()
{
    digitalWrite(buzzerPin, HIGH);
    delay(300);
    digitalWrite(buzzerPin, LOW);
    delay(200);
    digitalWrite(buzzerPin, HIGH);
    delay(300);
    digitalWrite(buzzerPin, LOW);
}
