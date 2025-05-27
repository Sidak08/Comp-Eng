// ESP32 Heads Up Game
// Shows random animal words on LCD with button input to advance
// Player holds LCD away from them while others give clues

#include <Arduino.h>
#include <LiquidCrystal.h> // Library for controlling LCD display

// LCD pin connections (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

// Pin definitions
const int buttonPin = 22; // Button input
const int buzzerPin = 23; // Buzzer output
int buttonPressTime = 0;  // Button timing

long timeLimit = 15000; // Time limit for the player to guess each word (15 seconds)
long startTime = 0;     // Used to measure time that has passed for each word
int roundNumber = 0;    // Keeps track of the roundNumber so that it can be displayed at the end of the game
const int arraySize = 25;

// Animal word list
const char *words[arraySize] = {
    "moose", "beaver", "bear", "goose", "dog",
    "cat", "squirrel", "bird", "elephant", "horse",
    "bull", "giraffe", "seal", "bat", "skunk",
    "turtle", "whale", "rhino", "lion", "monkey",
    "frog", "alligator", "kangaroo", "hippo", "rabbit"};

// Array to hold randomized sequence of word indices
int sequence[arraySize];

// Function prototypes
void showStartSequence();
void generateRandomOrder();
void gameOver();
void winner();

void setup()
{
    pinMode(buttonPin, INPUT_PULLUP);

    lcd.begin(16, 2);

    Serial.begin(9600);
    Serial.println("ESP32 Heads Up Game");

    // Initialize sequence array
    for (int i = 0; i < arraySize; i++)
    {
        sequence[i] = -1;
    }

    // Initialize random generator
    randomSeed(analogRead(34));

    generateRandomOrder();
    showStartSequence();
}

void loop()
{
    for (int i = 0; i < arraySize; i++)
    {
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
        while (digitalRead(buttonPin) == HIGH)
        {
            int roundedTime = round((timeLimit - (millis() - startTime)) / 1000);
            lcd.setCursor(14, 1);
            lcd.print("  ");
            lcd.setCursor(14, 1);
            lcd.print(roundedTime);
            delay(15);

            // Check for timeout
            if (millis() - startTime > timeLimit)
            {
                gameOver();
            }

            // Button feedback
            if (digitalRead(buttonPin) == LOW)
            {
                tone(buzzerPin, 272, 10);
            }
        }

        delay(500); // Debounce delay
    }

    // All words completed successfully
    winner();
}

//--------------FUNCTIONS------------------------------

// Game startup sequence with countdown
void showStartSequence()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Category:");
    lcd.setCursor(0, 1);
    lcd.print("Animals");
    delay(2000);

    lcd.clear();
    lcd.print("Get ready!");
    delay(1000);

    lcd.clear();
    lcd.print("3");
    delay(1000);

    lcd.clear();
    lcd.print("2");
    delay(1000);

    lcd.clear();
    lcd.print("1");
    delay(1000);
}

// Create randomized word order sequence
void generateRandomOrder()
{
    Serial.println("Generating random order...");

    for (int i = 0; i < arraySize; i++)
    {                          // Fill all 25 positions
        int currentNumber = 0; // Variable to hold the current number
        boolean match = false; // Does currentNumber match any previous numbers?

        // Generate random numbers until we get one that doesn't match any in the array
        do
        {
            currentNumber = random(0, arraySize); // Generate a random number from 0 to 24
            match = false;                        // Start by assuming it doesn't match

            // Check against all numbers already in the sequence
            for (int j = 0; j < arraySize; j++)
            {
                if (currentNumber == sequence[j])
                {
                    match = true; // If it matches, set match flag
                    break;        // No need to check further
                }
            }
        } while (match == true); // Try again if we found a match

        sequence[i] = currentNumber; // Add unique number to sequence
        Serial.print(sequence[i]);   // Debug: print the sequence
        Serial.print(" ");
    }
    Serial.println(); // End of sequence debug output
}

// Handle game over condition
void gameOver()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game Over");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(roundNumber);

    Serial.print("Game Over! Final Score: ");
    Serial.println(roundNumber);

    // Play lose melody
    tone(buzzerPin, 130, 250);
    delay(275);
    tone(buzzerPin, 73, 250);
    delay(275);
    tone(buzzerPin, 65, 150);
    delay(175);
    tone(buzzerPin, 98, 500);
    delay(500);

    while (true)
    {
    } // Loop indefinitely
}

// Handle game win condition
void winner()
{
    lcd.clear();
    lcd.setCursor(7, 0);
    lcd.print("YOU");
    lcd.setCursor(7, 1);
    lcd.print("WIN!");

    Serial.println("YOU WIN!");

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

    while (true)
    {
    } // Loop indefinitely
}
