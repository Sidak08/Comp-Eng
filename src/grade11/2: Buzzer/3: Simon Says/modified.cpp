#include <Arduino.h>

int button[] = {4, 13, 14, 33}; // Red, yellow, green, blue buttons
int led[] = {5, 12, 18, 22};    // Red, yellow, green, blue LEDs
int tones[] = {262, 330, 392, 494}; // C, E, G, B tones

int levelDisplayPins[] = {2, 15, 3, 0, 19, 21, 23, 26, 27, 32};

int roundsToWin = 10;
int buttonSequence[16];

int buzzerPin = 25;

int pressedButton = 4;
int roundCounter = 1;

long startTime = 0;
long timeLimit = 2000;

boolean gameStarted = false;

void flashLED(int ledNumber);
void allLEDoff();
int buttonCheck();
void startSequence();
void winSequence();
void loseSequence();
void updateLevelDisplay();
void clearLevelDisplay();
void testLevelDisplay();

void setup()
{
    pinMode(button[0], INPUT_PULLUP);
    pinMode(button[1], INPUT_PULLUP);
    pinMode(button[2], INPUT_PULLUP);
    pinMode(button[3], INPUT_PULLUP);

    pinMode(led[0], OUTPUT);
    pinMode(led[1], OUTPUT);
    pinMode(led[2], OUTPUT);
    pinMode(led[3], OUTPUT);

    for (int i = 0; i < 10; i++)
    {
        pinMode(levelDisplayPins[i], OUTPUT);
        digitalWrite(levelDisplayPins[i], LOW);
        delay(10);
    }

    testLevelDisplay();

    pinMode(buzzerPin, OUTPUT);

    pinMode(3, OUTPUT);
    digitalWrite(3, LOW);
}

void loop()
{
    if (gameStarted == false)
    {
        startSequence();
        roundCounter = 0;
        clearLevelDisplay();
        updateLevelDisplay();
        delay(1500);
        gameStarted = true;
    }

    for (int i = 0; i <= roundCounter; i++)
    {
        flashLED(buttonSequence[i]);
        delay(200);
        allLEDoff();
        delay(200);
    }

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
        roundCounter = roundCounter + 1;
        clearLevelDisplay();
        updateLevelDisplay();

        if (roundCounter >= roundsToWin)
            winSequence();
        }

        delay(500);
    }
}

void testLevelDisplay()
{
    clearLevelDisplay();
    delay(300);

    for (int i = 0; i < 10; i++)
    {
        digitalWrite(levelDisplayPins[i], HIGH);
        tone(buzzerPin, 200 + (i * 100), 100);
        delay(400);
    }

    for (int i = 9; i >= 0; i--)
    {
        digitalWrite(levelDisplayPins[i], LOW);
        tone(buzzerPin, 1200 - (i * 100), 100);
        delay(150);
    }

    for (int wave = 0; wave < 3; wave++)
    {
        for (int i = 0; i < 10; i++)
        {
            digitalWrite(levelDisplayPins[i], HIGH);
            delay(50);
        }

        for (int i = 0; i < 10; i++)
        {
            digitalWrite(levelDisplayPins[i], LOW);
            delay(50);
        }
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 1; j < 10; j += 2)
        {
            digitalWrite(levelDisplayPins[j], HIGH);
        }
        tone(buzzerPin, 500, 100);
        delay(250);

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

void clearLevelDisplay()
{
    for (int i = 0; i < 10; i++)
    {
        digitalWrite(levelDisplayPins[i], LOW);
    }
    noTone(buzzerPin);
    delay(50);
}

void updateLevelDisplay()
{
    for (int i = 0; i < 10; i++)
    {
        digitalWrite(levelDisplayPins[i], i < roundCounter ? HIGH : LOW);
    }
}

void flashLED(int ledNumber)
{
    digitalWrite(led[ledNumber], HIGH);
    tone(buzzerPin, tones[ledNumber]);
}

void allLEDoff()
{
    digitalWrite(led[0], LOW);
    digitalWrite(led[1], LOW);
    digitalWrite(led[2], LOW);
    digitalWrite(led[3], LOW);
    noTone(buzzerPin);
}

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
        return 4; 
}

void startSequence()
{
    randomSeed(analogRead(34));

    for (int i = 0; i <= roundsToWin; i++)
    {
        buttonSequence[i] = round(random(0, 4));
    }

    for (int i = 0; i <= 3; i++)
    {
        tone(buzzerPin, tones[i], 200);

    
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

    tone(buzzerPin, 130, 250);
    delay(275);
    tone(buzzerPin, 73, 250);
    delay(275);
    tone(buzzerPin, 65, 150);
    delay(175);
    tone(buzzerPin, 98, 500);
    delay(500);

    do
    {
        pressedButton = buttonCheck();
    } while (pressedButton > 3);
    delay(200);

    gameStarted = false;
}
