// ESP32 Buzzer Example - Controls buzzer with button input

#include <Arduino.h>

// Pin definitions
int speakerPin = 25; // Buzzer pin
const int buttonPin = 13; // Button pin

// Function prototype
void play(char note, int beats);

void setup()
{
    Serial.begin(9600);
    pinMode(speakerPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    Serial.println("ESP32 started");
}

void loop()
{
    int state = digitalRead(buttonPin); // Read button state
    Serial.println(state);

    // Play "Happy Birthday" song
    play('g', 2); // ha-
    play('g', 1); // ppy
    play('a', 4); // birth-
    play('g', 4); // day
    play('C', 4); // to
    play('b', 4); // you

    play(' ', 2); // pause

    play('g', 2); // ha
    play('g', 1); // ppy
    play('a', 4); // birth
    play('g', 4); // day
    play('D', 4); // to
    play('C', 4); // you

    play(' ', 2); // pause for 2 beats

    play('g', 2); // ha
    play('g', 1); // ppy
    play('G', 4); // birth
    play('E', 4); // day
    play('C', 4); // dear
    play('b', 4); // your
    play('a', 6); // name

    play(' ', 2); // pause for 2 beats

    play('F', 2); // ha
    play('F', 1); // ppy
    play('E', 4); // birth
    play('C', 4); // day
    play('D', 4); // to
    play('C', 6); // you

    while (true)
    {
    } // Loop indefinitely to play song only once
}

void play(char note, int beats)
{
    int numNotes = 14; // Number of notes in array
    
    // Arrays for note lookup (C major scale)
    char notes[] = {'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', 'D', 'E', 'F', 'G', 'A', 'B', ' '};
    int frequencies[] = {131, 147, 165, 175, 196, 220, 247, 262, 294, 330, 349, 392, 440, 494, 0};

    int currentFrequency = 0;
    int beatLength = 150; // Beat duration in milliseconds

    // Find the matching frequency for the note
    for (int i = 0; i < numNotes; i++)
    {
        if (notes[i] == note)
        {
            currentFrequency = frequencies[i];
        }
    }

    // Play the note
    tone(speakerPin, currentFrequency, beats * beatLength);
    delay(beats * beatLength);
    delay(50); // Small delay between notes
}

// Notes: lowercase = lower octave, uppercase = higher octave
