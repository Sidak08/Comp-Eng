#include <Arduino.h>
#include <Servo.h>

// Forward declarations (needed in .cpp)
void setStepper1(int stepIndex);
void setStepper2(int stepIndex);

// --- Stepper 1 pins (3s delay) ---
int IN1 = 8;
int IN2 = 9;
int IN3 = 10;
int IN4 = 11;

// --- Stepper 2 pins (4s delay) ---
int IN5 = 2;
int IN6 = 3;
int IN7 = 4;
int IN8 = 5;

// --- Parameters ---
const unsigned long stepDelay1 = 3.5; // Stepper 1 speed (ms per step)
const unsigned long stepDelay2 = 2; // Stepper 2 speed (ms per step)

// --- State variables ---
int stepIndex1 = 0;
int stepIndex2 = 0;
int direction1 = 1; // stepper 1 direction
int direction2 = 1; // stepper 2 direction

unsigned long lastStepTime1 = 0;
unsigned long lastStepTime2 = 0;
unsigned long lastDirChange1 = 0;
unsigned long lastDirChange2 = 0;

// --- Step sequence (full-step drive) ---
int stepSequence[4][4] = {
  {1, 0, 1, 0},  // Step 1
  {0, 1, 1, 0},  // Step 2
  {0, 1, 0, 1},  // Step 3
  {1, 0, 0, 1}   // Step 4
};

// --- Servo ---
Servo myServo;
int servoPin = 12;

void setup() {
  // Stepper 1
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Stepper 2
  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);
  pinMode(IN7, OUTPUT);
  pinMode(IN8, OUTPUT);

  // Servo
  myServo.attach(servoPin);
}

void loop() {
  unsigned long currentTime = millis();

  // --- Stepper 1: Reverse every 3 seconds ---
  if (currentTime - lastDirChange1 >= 5000) {
    direction1 *= -1;
    lastDirChange1 = currentTime;
  }

  // --- Stepper 2: Reverse every 4 seconds ---
  if (currentTime - lastDirChange2 >= 4000) {
    direction2 *= -1;
    lastDirChange2 = currentTime;
  }

  // --- Stepper 1 movement (5 ms per step) ---
  if (currentTime - lastStepTime1 >= stepDelay1) {
    stepIndex1 = (stepIndex1 + direction1 + 4) % 4;
    setStepper1(stepIndex1);
    lastStepTime1 = currentTime;
  }

  // --- Stepper 2 movement (2 ms per step) ---
  if (currentTime - lastStepTime2 >= stepDelay2) {
    stepIndex2 = (stepIndex2 + direction2 + 4) % 4;
    setStepper2(stepIndex2);
    lastStepTime2 = currentTime;
  }

  // --- Servo continuous spin ---
  myServo.write(180); // 0 = reverse, 90 = stop, 180 = forward
}

// --- Helper functions ---
void setStepper1(int stepIndex) {
  digitalWrite(IN1, stepSequence[stepIndex][0]);
  digitalWrite(IN2, stepSequence[stepIndex][1]);
  digitalWrite(IN3, stepSequence[stepIndex][2]);
  digitalWrite(IN4, stepSequence[stepIndex][3]);
}

void setStepper2(int stepIndex) {
  digitalWrite(IN5, stepSequence[stepIndex][0]);
  digitalWrite(IN6, stepSequence[stepIndex][1]);
  digitalWrite(IN7, stepSequence[stepIndex][2]);
  digitalWrite(IN8, stepSequence[stepIndex][3]);
}
