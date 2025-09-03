int pin1 = 13;
int pin2 = 12;
int pin3 = 11;
int pin4 = 10;
int pin5 = 9;
int pin6 = 8;
int pin7 = 7;
int pin8 = 6;

int buttonPin = 2;

int name[5][7] = {
  {1, 0, 1, 0, 0, 1, 1},
  {1, 1, 0, 1, 0, 0, 1},
  {1, 1, 0, 0, 1, 0, 0},
  {1, 1, 0, 0, 0, 0, 1},
  {1, 1, 0, 1, 0, 1, 1}
};

int pins[8] = {pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8};

int currentPattern = 0;
bool lastButtonState = HIGH;
bool buttonState = HIGH;

void setup()
{
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  pinMode(pin5, OUTPUT);
  pinMode(pin6, OUTPUT);
  pinMode(pin7, OUTPUT);
  pinMode(pin8, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);

  displayPattern(currentPattern);
}

void loop(){
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
    digitalWrite(pins[k], LOW);
  }

  for (int j = 0; j < 7; j++) {
    digitalWrite(pins[j], name[patternIndex][j]);
  }

  digitalWrite(pin8, LOW);
}
