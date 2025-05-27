// ESP32 Simon Says Game
// The Simon Says game flashes a pattern using LED lights, then the player must repeat the pattern.

#include <Arduino.h>

// Pin definitions for buttons, LEDs and buzzer
int button[] = {4, 13, 14, 33}; // Red, yellow, green, blue buttons
int led[] = {5, 12, 18, 22};    // Red, yellow, green, blue LEDs
int tones[] = {262, 330, 392, 494}; // C, E, G, B notes

int roundsToWin = 10;   // Number of rounds needed to win
int buttonSequence[16]; // Sequence the player needs to remember

int buzzerPin = 25;     // Buzzer output pin

int pressedButton = 4;  // 4 = no button pressed
int roundCounter = 1;   // Current game round

long startTime = 0;     // Timer for button press limit
long timeLimit = 2000;  // Time limit in milliseconds

boolean gameStarted = false; // Control game start sequence

void flashLED(int ledNumber);
void allLEDoff();
int buttonCheck();
void startSequence();
void winSequence();
void loseSequence();

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

  pinMode(buzzerPin, OUTPUT); // Configure buzzer pin
}

void loop()
{
  if (gameStarted == false)
  {
    startSequence();    // Play start sequence
    roundCounter = 0;   // Reset round counter
    delay(1500);        // Wait before starting game
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
    roundCounter++; // Increment round counter

    if (roundCounter >= roundsToWin)
    {
      winSequence(); // Player won the game
    }

    delay(500); // Brief pause between rounds
  }
}

//----------FUNCTIONS------------

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
  randomSeed(analogRead(A0));
  
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
