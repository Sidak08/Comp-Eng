/*
  SparkFun Inventor's Kit - Modified for ESP32
  Circuit 4C - Heads Up Game

  This is a DIY version of the popular Heads Up party game. To play, one person resets the ESP32 and holds the LCD
  facing away from them so that they cannot see it (usually on their forehead). The display will show a short countdown
  then display random words. The other player(s) who can see the screen must yell out clues until time runs out or the player
  guesses what word is on the screen. If they guess correctly, they can press the button on the breadboard and another word
  will be displayed.

  Adapted for ESP32 from original SparkFun Electronics code.
*/

#include <Arduino.h>
#include <LiquidCrystal.h> // The liquid crystal library contains commands for printing to the display

// Define the pins connected to the LCD for ESP32
// RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 14, 27, 26, 25); // Tell the ESP32 what pins are connected to the display

const int buttonPin = 22; // Pin that the button is connected to
const int buzzerPin = 23; // Pin for driving the buzzer
int buttonPressTime = 0;  // Variable to show how much time the player has left to guess the word (and press the button)

long timeLimit = 15000; // Time limit for the player to guess each word (15 seconds)
long startTime = 0;     // Used to measure time that has passed for each word
int roundNumber = 0;    // Keeps track of the roundNumber so that it can be displayed at the end of the game
const int arraySize = 25;

// Array of words to be displayed
const char *words[arraySize] = {
    "moose", "beaver", "bear", "goose", "dog",
    "cat", "squirrel", "bird", "elephant", "horse",
    "bull", "giraffe", "seal", "bat", "skunk",
    "turtle", "whale", "rhino", "lion", "monkey",
    "frog", "alligator", "kangaroo", "hippo", "rabbit"};

// The start value in the sequence array must have a value that could never be an index of an array
// or at least a value outside the range of 0 to the size of the words array - 1
int sequence[arraySize]; // Will be filled with random indices

// Function prototypes
void showStartSequence();
void generateRandomOrder();
void gameOver();
void winner();

void setup()
{
    pinMode(buttonPin, INPUT_PULLUP); // Set the button pin as an input with pullup

    // Initialize the LCD
    lcd.begin(16, 2); // Tell the LCD library the size of the screen

    Serial.begin(9600); // Initialize serial communication for debugging
    Serial.println("ESP32 Heads Up Game");

    // Initialize the sequence array with -1 values
    for (int i = 0; i < arraySize; i++)
    {
        sequence[i] = -1;
    }

    // Reset the random seed for better randomness
    randomSeed(analogRead(34)); // Use an unconnected analog pin for noise

    generateRandomOrder(); // Generate an array of random numbers from 0 to 24
    showStartSequence();   // Print the start sequence text
}

void loop()
{
    for (int i = 0; i < arraySize; i++)
    {                // For each of the 25 words in the sequence
        lcd.clear(); // Clear the screen

        roundNumber = i + 1;           // The array starts at 0, but the roundNumber starts counting from 1
        lcd.print(roundNumber);        // Print the current round number
        lcd.print(": ");               // Spacer between the number and the word
        lcd.print(words[sequence[i]]); // Print a random word from the word array

        Serial.print("Round ");
        Serial.print(roundNumber);
        Serial.print(": ");
        Serial.println(words[sequence[i]]); // Debug output

        startTime = millis(); // Record the time that this round started

        // Loop until the button is pressed
        while (digitalRead(buttonPin) == HIGH)
        {
            int roundedTime = round((timeLimit - (millis() - startTime)) / 1000); // Calculate time left in seconds
            lcd.setCursor(14, 1);                                                 // Set the cursor in the lower right corner of the screen
            lcd.print("  ");                                                      // Clear the previous time
            lcd.setCursor(14, 1);
            lcd.print(roundedTime); // Print the time left
            delay(15);              // Small delay for stability

            // Check if time is up
            if (millis() - startTime > timeLimit)
            {
                gameOver(); // End the game if time runs out
            }

            // Beep when button is pressed
            if (digitalRead(buttonPin) == LOW)
            {
                tone(buzzerPin, 272, 10); // Emit a short beep
            }
        }

        // Button was pressed, wait before continuing to next word
        delay(500); // Prevent button bounce
    }

    // If all 25 words are completed
    winner(); // Show the winning message
}

//--------------FUNCTIONS------------------------------

// DISPLAYS A COUNTDOWN TO START THE GAME
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

// GENERATES A RANDOM ORDER FOR THE WORDS TO BE DISPLAYED
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

// GAME OVER
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

    // Play the losing fog horn
    tone(buzzerPin, 130, 250); // E6
    delay(275);
    tone(buzzerPin, 73, 250); // G6
    delay(275);
    tone(buzzerPin, 65, 150); // E7
    delay(175);
    tone(buzzerPin, 98, 500); // C7
    delay(500);

    while (true)
    {
    } // Get stuck in this loop forever
}

// WINNER
void winner()
{
    lcd.clear();
    lcd.setCursor(7, 0);
    lcd.print("YOU");
    lcd.setCursor(7, 1);
    lcd.print("WIN!");

    Serial.println("YOU WIN!");

    // Play the 1Up noise
    tone(buzzerPin, 1318, 150); // E6
    delay(175);
    tone(buzzerPin, 1567, 150); // G6
    delay(175);
    tone(buzzerPin, 2637, 150); // E7
    delay(175);
    tone(buzzerPin, 2093, 150); // C7
    delay(175);
    tone(buzzerPin, 2349, 150); // D7
    delay(175);
    tone(buzzerPin, 3135, 500); // G7
    delay(500);

    while (true)
    {
    } // Get stuck in this loop forever
}
