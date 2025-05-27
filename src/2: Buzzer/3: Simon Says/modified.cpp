// ESP32 Simon Says Game with 10-Segment Level Display
// The game flashes a pattern using LED lights, then the player must repeat the pattern.
// This version includes a 10-segment level display to show the current round.

#include <Arduino.h>

// Pin definitions for buttons, LEDs and buzzer
int button[] = {4, 13, 14, 33}; // Red, yellow, green, blue buttons
int led[] = {5, 12, 18, 22};    // Red, yellow, green, blue LEDs
int tones[] = {262, 330, 392, 494}; // C, E, G, B tones

// Pins for the 10-segment level display
int levelDisplayPins[] = {2, 15, 3, 0, 19, 21, 23, 26, 27, 32};

int roundsToWin = 10;   // number of rounds the player has to play before they win the game (the array can only hold up to 16 rounds)
int buttonSequence[16]; // make an array of numbers that will be the sequence that the player needs to remember

int buzzerPin = 25; // pin that the buzzer is connected to

int pressedButton = 4; // a variable to remember which button is being pressed. 4 is the value if no button is being pressed.
int roundCounter = 1;  // keeps track of what round the player is on

long startTime = 0;    // timer variable for time limit on button press
long timeLimit = 2000; // time limit to hit a button

boolean gameStarted = false; // variable to tell the game whether or not to play the start sequence

void flashLED(int ledNumber);
void allLEDoff();
int buttonCheck();
void startSequence();
void winSequence();
void loseSequence();
void updateLevelDisplay(); // Function to update the 10-segment display
void clearLevelDisplay();  // Function to clear all segments of the level display
void testLevelDisplay();   // Function to test all segments of the level display

void setup()
{
    // Configure button pins with internal pull-up resistors
    pinMode(button[0], INPUT_PULLUP);
    pinMode(button[1], INPUT_PULLUP);
    pinMode(button[2], INPUT_PULLUP);
    pinMode(button[3], INPUT_PULLUP);

    // Configure LED pins as outputs
    pinMode(led[0], OUTPUT);
    pinMode(led[1], OUTPUT);
    pinMode(led[2], OUTPUT);
    pinMode(led[3], OUTPUT);

    // Configure level display pins as outputs
    for (int i = 0; i < 10; i++)
    {
        pinMode(levelDisplayPins[i], OUTPUT);
        digitalWrite(levelDisplayPins[i], LOW); // Ensure all segments are off initially
        delay(10);                              // Small delay between pin initializations
    }

    // Test the level display segments
    testLevelDisplay();

    pinMode(buzzerPin, OUTPUT); // Configure buzzer pin as output
    
    pinMode(3, OUTPUT);
    digitalWrite(3, LOW);
}

void loop()
{
    if (gameStarted == false)
    { 
        startSequence();      // Play start sequence
        roundCounter = 0;     // Reset round counter
        clearLevelDisplay();  // Clear level display
        updateLevelDisplay(); // Update level display
        delay(1500);
        gameStarted = true;
    }

    // Flash the sequence to be repeated
    for (int i = 0; i <= roundCounter; i++)
    {
        flashLED(buttonSequence[i]); // Flash LED and play tone
        delay(200);
        allLEDoff();
        delay(200);
    }

    // Check player's button presses against the sequence
    for (int i = 0; i <= roundCounter; i++)
    {
        startTime = millis();

        while (gameStarted == true)
        {
            pressedButton = buttonCheck();

            if (pressedButton < 4)
            { // Button is pressed
                flashLED(pressedButton);

                if (pressedButton == buttonSequence[i])
                { // Correct button
                    delay(250);
                    allLEDoff();
                    break;
                }
                else
                { // Wrong button
                    loseSequence();
                    break;
                }
            }
            else
            {
                allLEDoff();
            }

            // Check time limit
            if (millis() - startTime > timeLimit)
            {
                loseSequence();
                break;
            }
        }
    }

    if (gameStarted == true)
    {
        roundCounter = roundCounter + 1; // increase the round number by 1
        // Serial.print("Advancing to round: ");
        // Serial.println(roundCounter);
        clearLevelDisplay();  // Clear the display first
        updateLevelDisplay(); // Update the level display

        if (roundCounter >= roundsToWin)
        {                  // if the player has gotten to the 16th round
            winSequence(); // play the winning song
        }

        delay(500); // wait for half a second between rounds
    }
}

// Test the level display segments
void testLevelDisplay()
{
    clearLevelDisplay();
    delay(300);

    // Light each segment with ascending tones
    for (int i = 0; i < 10; i++)
    {
        digitalWrite(levelDisplayPins[i], HIGH);
        tone(buzzerPin, 200 + (i * 100), 100);
        delay(400);
    }

    // Turn off segments with descending tones
    for (int i = 9; i >= 0; i--)
    {
        digitalWrite(levelDisplayPins[i], LOW);
        tone(buzzerPin, 1200 - (i * 100), 100);
        delay(150);
    }

    // Wave pattern (3 times)
    for (int wave = 0; wave < 3; wave++)
    {
        // Light up in sequence
        for (int i = 0; i < 10; i++)
        {
            digitalWrite(levelDisplayPins[i], HIGH);
            delay(50);
        }

        // Turn off in sequence
        for (int i = 0; i < 10; i++)
        {
            digitalWrite(levelDisplayPins[i], LOW);
            delay(50);
        }
    }

    // Alternate odd/even pattern
    for (int i = 0; i < 3; i++)
    {
        // Light odd segments
        for (int j = 1; j < 10; j += 2)
        {
            digitalWrite(levelDisplayPins[j], HIGH);
        }
        tone(buzzerPin, 500, 100);
        delay(250);

        // Switch to even segments
        for (int j = 0; j < 10; j++)
        {
            digitalWrite(levelDisplayPins[j], j % 2 == 0);
        }
        tone(buzzerPin, 700, 100);
        delay(250);
    }

    clearLevelDisplay();
    delay(500);
}

// Clear all segments of the level display
void clearLevelDisplay()
{
    for (int i = 0; i < 10; i++)
    {
        digitalWrite(levelDisplayPins[i], LOW);
    }
    noTone(buzzerPin);
    delay(50);
}

// Update the level display to show current round
void updateLevelDisplay()
{
    for (int i = 0; i < 10; i++)
    {
        digitalWrite(levelDisplayPins[i], i < roundCounter ? HIGH : LOW);
    }
}

// Core game functions

// Flash LED and play corresponding tone
void flashLED(int ledNumber)
{
    digitalWrite(led[ledNumber], HIGH);
    tone(buzzerPin, tones[ledNumber]);
}

// Turn all LEDs off and stop sound
void allLEDoff()
{
    digitalWrite(led[0], LOW);
    digitalWrite(led[1], LOW);
    digitalWrite(led[2], LOW);
    digitalWrite(led[3], LOW);
    noTone(buzzerPin);
}

// Check which button is currently pressed
int buttonCheck()
{
    if (digitalRead(button[0]) == LOW)
        return 0;
    else if (digitalRead(button[1]) == LOW)
        return 1;
    else if (digitalRead(button[2]) == LOW)
        return 2;
    else if (digitalRead(button[3]) == LOW)
        return 3;
    else
        return 4; // No button pressed
}

// Game start sequence
void startSequence()
{
    // Generate random pattern
    randomSeed(analogRead(34));
    
    for (int i = 0; i <= roundsToWin; i++)
    {
        buttonSequence[i] = round(random(0, 4));
    }

    // Flash startup animation
    for (int i = 0; i <= 3; i++)
    {
        tone(buzzerPin, tones[i], 200);

        // Turn all LEDs on
        for (int j = 0; j < 4; j++) {
            digitalWrite(led[j], HIGH);
        }
        
        delay(100);
        
        // Turn all LEDs off
        for (int j = 0; j < 4; j++) {
            digitalWrite(led[j], LOW);
        }
        
        delay(100);
    }
}

// Game win sequence
void winSequence()
{
    // Turn all LEDs on
    for (int j = 0; j <= 3; j++)
    {
        digitalWrite(led[j], HIGH);
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

    // Wait for button press to restart
    do
    {
        pressedButton = buttonCheck();
    } while (pressedButton > 3);
    delay(100);

    gameStarted = false;
}

// Game lose sequence
void loseSequence()
{
    // Turn all LEDs on
    for (int j = 0; j <= 3; j++)
    {
        digitalWrite(led[j], HIGH);
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

    // Wait for button press to restart
    do
    {
        pressedButton = buttonCheck();
    } while (pressedButton > 3);
    delay(200);

    gameStarted = false;
}
