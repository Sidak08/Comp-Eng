int pin1 = 13;
int pin2 = 12;
int pin3 = 11;
int pin4 = 10;
int pin5 = 9;
int pin6 = 8;
int pin7 = 7;
int pin8 = 6;

int buttonPin = 2;

int name[5][8] = {
  {0, 1, 0, 1, 0, 0, 1, 1},
  {0, 1, 1, 0, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 0, 0},
  {0, 1, 1, 0, 0, 0, 0, 1},
  {0, 1, 1, 0, 1, 0, 1, 1}
};

int pins[8] = {pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8};

int currentPattern = 0;
bool lastButtonState = HIGH;
bool buttonState = HIGH;

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(pins[i], OUTPUT);
  }

  pinMode(buttonPin, INPUT_PULLUP);

  displayPattern(currentPattern);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && buttonState == LOW) {
    currentPattern++;
    if (currentPattern >= 5) {
      currentPattern = 0;
    }

    displayPattern(currentPattern);

    delay(200);
  }

  lastButtonState = buttonState;
}

void displayPattern(int patternIndex) {
  for (int k = 0; k < 8; k++) {
    digitalWrite(pins[k], name[patternIndex][k]);
  }
}
