/*
  SparkFun Inventor's Kit - Modified for ESP32
  Circuit 4C - Heads Up Game with RGB LED Countdown
  
  This is a DIY version of the popular Heads Up party game with an RGB LED countdown timer.
  The RGB LED changes color as time runs out: green → yellow → orange → red.
  
  Adapted for ESP32 from original SparkFun Electronics code.
*/

#include <Arduino.h>
#include <LiquidCrystal.h>   // The liquid crystal library contains commands for printing to the display

// Define the pins connected to the LCD for ESP32
// RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);   // Tell the ESP32 what pins are connected to the display

const int buttonPin = 22;    // Pin that the button is connected to
const int buzzerPin = 23;    // Pin for driving the buzzer

// RGB LED pins
const int redPin = 19;       // Red component of RGB LED
const int greenPin = 18;     // Green component of RGB LED
const int bluePin = 5;       // Blue component of RGB LED

// PWM properties for RGB LED
const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;    // 8-bit resolution (0-255)

int buttonPressTime = 0;     // Variable to show how much time the player has left to guess the word (and press the button)

long timeLimit = 15000;      // Time limit for the player to guess each word (15 seconds)
long startTime = 0;          // Used to measure time that has passed for each word
int roundNumber = 0;         // Keeps track of the roundNumber so that it can be displayed at the end of the game
const int arraySize = 25;

// Array of words to be displayed
const char* words[arraySize] = {
  "moose", "beaver", "bear", "goose", "dog", 
  "cat", "squirrel", "bird", "elephant", "horse",
  "bull", "giraffe", "seal", "bat", "skunk", 
  "turtle", "whale", "rhino", "lion", "monkey",
  "frog", "alligator", "kangaroo", "hippo", "rabbit"
};

// The start value in the sequence array must have a value that could never be an index of an array
// or at least a value outside the range of 0 to the size of the words array - 1
int sequence[arraySize]; // Will be filled with random indices

// Function prototypes
void showStartSequence();
void generateRandomOrder();
void gameOver();
void winner();
void setRgbColor(int red, int green, int blue);
void updateRgbCountdown(int timeRemaining);

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);    // Set the button pin as an input with pullup
  
  // Initialize the LCD
  lcd.begin(16, 2);                    // Tell the LCD library the size of the screen
  
  Serial.begin(9600);                  // Initialize serial communication for debugging
  Serial.println("ESP32 Heads Up Game with RGB LED Countdown");
  
  // Configure RGB LED PWM channels
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);
  
  // Attach the PWM channels to the RGB LED pins
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  
  // Initialize all LED colors to OFF
  setRgbColor(0, 0, 0);
  
  // Initialize the sequence array with -1 values
  for (int i = 0; i < arraySize; i++) {
    sequence[i] = -1;
  }
  
  // Reset the random seed for better randomness
  randomSeed(analogRead(34));          // Use an unconnected analog pin for noise
  
  generateRandomOrder();               // Generate an array of random numbers from 0 to 24
  showStartSequence();                 // Print the start sequence text
}

void loop() {
  for (int i = 0; i < arraySize; i++) {   // For each of the 25 words in the sequence
    lcd.clear();                        // Clear the screen
    
    roundNumber = i + 1;                // The array starts at 0, but the roundNumber starts counting from 1
    lcd.print(roundNumber);             // Print the current round number
    lcd.print(": ");                    // Spacer between the number and the word
    lcd.print(words[sequence[i]]);      // Print a random word from the word array
    
    Serial.print("Round ");
    Serial.print(roundNumber);
    Serial.print(": ");
    Serial.println(words[sequence[i]]);  // Debug output
    
    startTime = millis();               // Record the time that this round started
    
    // Loop until the button is pressed
    while (digitalRead(buttonPin) == HIGH) {
      int timeRemaining = timeLimit - (millis() - startTime);
      int roundedTime = round(timeRemaining / 1000); // Calculate time left in seconds
      
      // Update the RGB LED based on remaining time
      updateRgbCountdown(timeRemaining);
      
      lcd.setCursor(14, 1);             // Set the cursor in the lower right corner of the screen
      lcd.print("  ");                  // Clear the previous time
      lcd.setCursor(14, 1);
      lcd.print(roundedTime);           // Print the time left
      delay(15);                        // Small delay for stability
      
      // Check if time is up
      if (millis() - startTime > timeLimit) {
        gameOver();                     // End the game if time runs out
      }
      
      // Beep when button is pressed
      if (digitalRead(buttonPin) == LOW) {
        tone(buzzerPin, 272, 10);       // Emit a short beep
      }
    }
    
    // Button was pressed, turn LED off and wait before continuing to next word
    setRgbColor(0, 0, 0);               // Turn off RGB LED
    delay(500);                         // Prevent button bounce
  }
  
  // If all 25 words are completed
  winner();                             // Show the winning message
}

//--------------FUNCTIONS------------------------------

// DISPLAYS A COUNTDOWN TO START THE GAME
void showStartSequence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Category:");
  lcd.setCursor(0, 1);
  lcd.print("Animals");
  
  // Flash RGB LED green during start sequence
  for (int i = 0; i < 4; i++) {
    setRgbColor(0, 255, 0);
    delay(250);
    setRgbColor(0, 0, 0);
    delay(250);
  }
  
  delay(1000);  // Additional delay to complete 2 seconds
  
  lcd.clear();
  lcd.print("Get ready!");
  
  // Start with blue light for "Get ready!"
  setRgbColor(0, 0, 255);
  delay(1000);
  
  lcd.clear();
  lcd.print("3");
  setRgbColor(0, 255, 0);  // Green for 3
  delay(1000);
  
  lcd.clear();
  lcd.print("2");
  setRgbColor(255, 255, 0);  // Yellow for 2
  delay(1000);
  
  lcd.clear();
  lcd.print("1");
  setRgbColor(255, 0, 0);  // Red for 1
  delay(1000);
  
  // Turn off LED when game starts
  setRgbColor(0, 0, 0);
}

// GENERATES A RANDOM ORDER FOR THE WORDS TO BE DISPLAYED
void generateRandomOrder() {
  Serial.println("Generating random order...");
  
  for (int i = 0; i < arraySize; i++) {  // Fill all 25 positions
    int currentNumber = 0;                // Variable to hold the current number
    boolean match = false;                // Does currentNumber match any previous numbers?
    
    // Generate random numbers until we get one that doesn't match any in the array
    do {
      currentNumber = random(0, arraySize);  // Generate a random number from 0 to 24
      match = false;                         // Start by assuming it doesn't match
      
      // Check against all numbers already in the sequence
      for (int j = 0; j < arraySize; j++) {
        if (currentNumber == sequence[j]) {
          match = true;                     // If it matches, set match flag
          break;                            // No need to check further
        }
      }
    } while (match == true);                // Try again if we found a match
    
    sequence[i] = currentNumber;            // Add unique number to sequence
    Serial.print(sequence[i]);              // Debug: print the sequence
    Serial.print(" ");
  }
  Serial.println();  // End of sequence debug output
}

// GAME OVER
void gameOver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(roundNumber);
  
  Serial.print("Game Over! Final Score: ");
  Serial.println(roundNumber);
  
  // Flash the red LED for game over
  for (int i = 0; i < 5; i++) {
    setRgbColor(255, 0, 0);  // Red
    delay(200);
    setRgbColor(0, 0, 0);    // Off
    delay(200);
  }
  
  // Play the losing fog horn
  tone(buzzerPin, 130, 250);   // E6
  delay(275);
  tone(buzzerPin, 73, 250);    // G6
  delay(275);
  tone(buzzerPin, 65, 150);    // E7
  delay(175);
  tone(buzzerPin, 98, 500);    // C7
  delay(500);
  
  // Keep LED red for game over state
  setRgbColor(255, 0, 0);
  
  while (true) {}              // Get stuck in this loop forever
}

// WINNER
void winner() {
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print("YOU");
  lcd.setCursor(7, 1);
  lcd.print("WIN!");
  
  Serial.println("YOU WIN!");
  
  // Rainbow effect for winner
  for (int i = 0; i < 3; i++) {
    // Cycle through some colors
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
  
  // Play the 1Up noise
  tone(buzzerPin, 1318, 150);  // E6
  delay(175);
  tone(buzzerPin, 1567, 150);  // G6
  delay(175);
  tone(buzzerPin, 2637, 150);  // E7
  delay(175);
  tone(buzzerPin, 2093, 150);  // C7
  delay(175);
  tone(buzzerPin, 2349, 150);  // D7
  delay(175);
  tone(buzzerPin, 3135, 500);  // G7
  delay(500);
  
  // Set LED to green for winner state
  setRgbColor(0, 255, 0);
  
  while (true) {}              // Get stuck in this loop forever
}

// SET RGB LED COLOR
void setRgbColor(int red, int green, int blue) {
  ledcWrite(redChannel, red);
  ledcWrite(greenChannel, green);
  ledcWrite(blueChannel, blue);
}

// UPDATE RGB LED BASED ON COUNTDOWN TIME
void updateRgbCountdown(int timeRemaining) {
  int percentage = (timeRemaining * 100) / timeLimit;  // Calculate percentage of time remaining
  
  // Use different colors based on time remaining
  if (percentage > 75) {
    // More than 75% time left - Green
    setRgbColor(0, 255, 0);
  } 
  else if (percentage > 50) {
    // 50-75% time left - Yellow-Green
    setRgbColor(128, 255, 0);
  }
  else if (percentage > 25) {
    // 25-50% time left - Yellow
    setRgbColor(255, 255, 0);
  }
  else if (percentage > 10) {
    // 10-25% time left - Orange
    setRgbColor(255, 128, 0);
  }
  else {
    // Less than 10% time left - Red
    // Make it flash when less than 5% time remains
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
