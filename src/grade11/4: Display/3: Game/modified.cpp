#include <Arduino.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

const int buttonPin = 22;
const int buzzerPin = 23;

// RGB LED connections
const int redPin = 19;
const int greenPin = 18;
const int bluePin = 5;

// PWM settings
const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

int buttonPressTime = 0;
long timeLimit = 15000;
long startTime = 0;
int roundNumber = 0;
const int arraySize = 25;

const char* words[arraySize] = {
  "moose", "beaver", "bear", "goose", "dog",
  "cat", "squirrel", "bird", "elephant", "horse",
  "bull", "giraffe", "seal", "bat", "skunk",
  "turtle", "whale", "rhino", "lion", "monkey",
  "frog", "alligator", "kangaroo", "hippo", "rabbit"
};

int sequence[arraySize];
void showStartSequence();
void generateRandomOrder();
void gameOver();
void winner();
void setRgbColor(int red, int green, int blue);
void updateRgbCountdown(int timeRemaining);

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

  lcd.begin(16, 2);

  Serial.begin(9600);

  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);

  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);

  setRgbColor(0, 0, 0);

  for (int i = 0; i < arraySize; i++) {
    sequence[i] = -1;
  }

  randomSeed(analogRead(34));

  generateRandomOrder();
  showStartSequence();
}

void loop() {
  for (int i = 0; i < arraySize; i++) {
    lcd.clear();

    roundNumber = i + 1;
    lcd.print(roundNumber);
    lcd.print(": ");
    lcd.print(words[sequence[i]]);


    startTime = millis();

    while (digitalRead(buttonPin) == HIGH) {
      int timeRemaining = timeLimit - (millis() - startTime);
      int roundedTime = round(timeRemaining / 1000);

      updateRgbCountdown(timeRemaining);

      lcd.setCursor(14, 1);
      lcd.print("  ");
      lcd.setCursor(14, 1);
      lcd.print(roundedTime);
      delay(15);

      if (millis() - startTime > timeLimit) {
        gameOver();
      }

      if (digitalRead(buttonPin) == LOW) {
        tone(buzzerPin, 272, 10);
      }
    }

    setRgbColor(0, 0, 0);
    delay(500);
  }

  winner();
}

void showStartSequence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Category:");
  lcd.setCursor(0, 1);
  lcd.print("Animals");

  for (int i = 0; i < 4; i++) {
    setRgbColor(0, 255, 0);
    delay(250);
    setRgbColor(0, 0, 0);
    delay(250);
  }

  delay(1000);

  lcd.clear();
  lcd.print("Get ready!");
  setRgbColor(0, 0, 255);
  delay(1000);

  lcd.clear();
  lcd.print("3");
  setRgbColor(0, 255, 0);
  delay(1000);

  lcd.clear();
  lcd.print("2");
  setRgbColor(255, 255, 0);
  delay(1000);

  lcd.clear();
  lcd.print("1");
  setRgbColor(255, 0, 0);
  delay(1000);

  setRgbColor(0, 0, 0);
}

void generateRandomOrder() {
  Serial.println("Generating random order...");

  for (int i = 0; i < arraySize; i++) {
    int currentNumber = 0;
    boolean match = false;

    do {
      currentNumber = random(0, arraySize);
      match = false;

      for (int j = 0; j < arraySize; j++) {
        if (currentNumber == sequence[j]) {
          match = true;
          break;
        }
      }
    } while (match == true);

    sequence[i] = currentNumber;
    Serial.print(sequence[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void gameOver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(roundNumber);

  Serial.print("Game Over! Final Score: ");
  Serial.println(roundNumber);

  for (int i = 0; i < 5; i++) {
    setRgbColor(255, 0, 0);
    delay(200);
    setRgbColor(0, 0, 0);
    delay(200);
  }

  tone(buzzerPin, 130, 250);
  delay(275);
  tone(buzzerPin, 73, 250);
  delay(275);
  tone(buzzerPin, 65, 150);
  delay(175);
  tone(buzzerPin, 98, 500);
  delay(500);

  setRgbColor(255, 0, 0);

  while (true) {}
}

void winner() {
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print("YOU");
  lcd.setCursor(7, 1);
  lcd.print("WIN!");

  Serial.println("YOU WIN!");

  for (int i = 0; i < 3; i++) {
    setRgbColor(255, 0, 0);    // Red
    delay(200);
    setRgbColor(255, 165, 0);  // Orange
    delay(200);
    setRgbColor(255, 255, 0);  // Yellow
    delay(200);
    setRgbColor(0, 255, 0);    // Green
    delay(200);
    setRgbColor(0, 0, 255);    // Blue
    delay(200);
    setRgbColor(75, 0, 130);   // Indigo
    delay(200);
    setRgbColor(238, 130, 238);// Violet
    delay(200);
  }

  // Play win melody
  tone(buzzerPin, 1318, 150);
  delay(175);
  tone(buzzerPin, 1567, 150);
  delay(175);
  tone(buzzerPin, 2637, 150);
  delay(175);
  tone(buzzerPin, 2093, 150);
  delay(175);
  tone(buzzerPin, 2349, 150);
  delay(175);
  tone(buzzerPin, 3135, 500);
  delay(500);

  setRgbColor(0, 255, 0);

  while (true) {}
}

void setRgbColor(int red, int green, int blue) {
  ledcWrite(redChannel, red);
  ledcWrite(greenChannel, green);
  ledcWrite(blueChannel, blue);
}

void updateRgbCountdown(int timeRemaining) {
  int percentage = (timeRemaining * 100) / timeLimit;

  if (percentage > 75) {
    setRgbColor(0, 255, 0);
  }
  else if (percentage > 50) {
    setRgbColor(128, 255, 0);
  }
  else if (percentage > 25) {
    setRgbColor(255, 255, 0);
  }
  else if (percentage > 10) {
    setRgbColor(255, 128, 0);
  }
  else {
    if (percentage < 5) {
      if (millis() % 300 < 150) {
        setRgbColor(255, 0, 0);
      } else {
        setRgbColor(0, 0, 0);
      }
    } else {
      setRgbColor(255, 0, 0);
    }
  }
}
