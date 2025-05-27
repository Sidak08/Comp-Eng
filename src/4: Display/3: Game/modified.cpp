// ESP32 Heads Up Game with RGB LED Countdown
// DIY version of the Heads Up party game with LED color changing timer
// RGB LED changes from green → yellow → orange → red as time runs out

#include <Arduino.h>
#include <LiquidCrystal.h>   // Library for controlling LCD display

// LCD pin connections (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

const int buttonPin = 22;    // Button input
const int buzzerPin = 23;    // Buzzer output

// RGB LED connections
const int redPin = 19;       // Red pin
const int greenPin = 18;     // Green pin
const int bluePin = 5;       // Blue pin

// PWM settings
const int freq = 5000;       // PWM frequency
const int redChannel = 0;    // Red PWM channel
const int greenChannel = 1;  // Green PWM channel
const int blueChannel = 2;   // Blue PWM channel
const int resolution = 8;    // 8-bit (0-255)

int buttonPressTime = 0;     // Time remaining to guess word
long timeLimit = 15000;      // Time limit per word (15 seconds)
long startTime = 0;          // Round start timestamp
int roundNumber = 0;         // Current round number
const int arraySize = 25;    // Total number of words

// Words for guessing (animal category)
const char* words[arraySize] = {
  "moose", "beaver", "bear", "goose", "dog", 
  "cat", "squirrel", "bird", "elephant", "horse",
  "bull", "giraffe", "seal", "bat", "skunk", 
  "turtle", "whale", "rhino", "lion", "monkey",
  "frog", "alligator", "kangaroo", "hippo", "rabbit"
};

// Will store randomized word order
int sequence[arraySize];

// Function prototypes
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
  Serial.println("ESP32 Heads Up Game with RGB LED Countdown");
  
  // Initialize PWM for RGB LED
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);
  
  // Connect PWM channels to pins
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  
  // Turn off RGB LED
  setRgbColor(0, 0, 0);
  
  // Initialize sequence array
  for (int i = 0; i < arraySize; i++) {
    sequence[i] = -1;
  }
  
  // Initialize random generator
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
    
    Serial.print("Round ");
    Serial.print(roundNumber);
    Serial.print(": ");
    Serial.println(words[sequence[i]]);
    
    startTime = millis();
    
    // Wait for button press or timeout
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
  
  // All words completed successfully
  winner();
}

// Core functions
// Core game functions

// Display game introduction and countdown
void showStartSequence() {
  // Show category
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Category:");
  lcd.setCursor(0, 1);
  lcd.print("Animals");
  
  // Green flash animation
  for (int i = 0; i < 4; i++) {
    setRgbColor(0, 255, 0);
    delay(250);
    setRgbColor(0, 0, 0);
    delay(250);
  }
  
  delay(1000);
  
  // Countdown sequence
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

// Create randomized word order sequence
void generateRandomOrder() {
  Serial.println("Generating random order...");
  
  for (int i = 0; i < arraySize; i++) {
    int currentNumber = 0;
    boolean match = false;
    
    // Find unique random number
    do {
      currentNumber = random(0, arraySize);
      match = false;
      
      // Check if number already exists in sequence
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

// Handle game over condition
void gameOver() {
  // Display game over and score
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(roundNumber);
  
  Serial.print("Game Over! Final Score: ");
  Serial.println(roundNumber);
  
  // Red LED flash animation
  for (int i = 0; i < 5; i++) {
    setRgbColor(255, 0, 0);
    delay(200);
    setRgbColor(0, 0, 0);
    delay(200);
  }
  
  // Play lose melody
  tone(buzzerPin, 130, 250);
  delay(275);
  tone(buzzerPin, 73, 250);
  delay(275);
  tone(buzzerPin, 65, 150);
  delay(175);
  tone(buzzerPin, 98, 500);
  delay(500);
  
  // Final state - red LED
  setRgbColor(255, 0, 0);
  
  while (true) {}
}

// Handle game win condition
void winner() {
  // Display win message
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print("YOU");
  lcd.setCursor(7, 1);
  lcd.print("WIN!");
  
  Serial.println("YOU WIN!");
  
  // Rainbow animation
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
  
  // Final state - green LED
  setRgbColor(0, 255, 0);
  
  while (true) {}
}

// Set RGB LED color values
void setRgbColor(int red, int green, int blue) {
  ledcWrite(redChannel, red);
  ledcWrite(greenChannel, green);
  ledcWrite(blueChannel, blue);
}

// Update LED color based on remaining time
void updateRgbCountdown(int timeRemaining) {
  int percentage = (timeRemaining * 100) / timeLimit;
  
  if (percentage > 75) {
    setRgbColor(0, 255, 0);      // Green (>75%)
  } 
  else if (percentage > 50) {
    setRgbColor(128, 255, 0);    // Yellow-Green (50-75%)
  }
  else if (percentage > 25) {
    setRgbColor(255, 255, 0);    // Yellow (25-50%)
  }
  else if (percentage > 10) {
    setRgbColor(255, 128, 0);    // Orange (10-25%)
  }
  else {
    // Red (<10%), flashing if under 5%
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
