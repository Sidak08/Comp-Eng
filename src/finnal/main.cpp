#include <Arduino.h>
#include <LiquidCrystal.h>

// Traffic Light Pins (output) [red, yellow, green]
const int trafficLight1[3] = {19, 2, 4};   // GPIO19, GPIO2, GPIO4
const int trafficLight2[3] = {5, 15, 18};  // GPIO5, GPIO15, GPIO18

// Speed checker pins (input) [sensor1, sensor2]
const int speedSensors[2] = {22, 23};      // GPIO22, GPIO23

// Pedestrian pins (not implemented in this version) [ped1, ped2]
const int pedestrians[2] = {32, 21};       // GPIO32, GPIO21

// Night light sensor (not implemented in this version)
const int nightSensor = 33;  // GPIO33

// LCD pins [RS, EN, D4, D5, D6, D7]
const int lcdPins[6] = {13, 12, 14, 27, 26, 25}; // GPIO13, GPIO12, GPIO14, GPIO27, GPIO26, GPIO25

// Traffic light timing (in milliseconds)
const int greenTime = 10000;
const int yellowTime = 3000;
const int redTime = 2000;
const int allRedTime = 2000;

// Traffic light states
const int STATE_1_GREEN = 0;
const int STATE_1_YELLOW = 1;
const int STATE_1_RED = 2;
const int STATE_2_GREEN = 3;
const int STATE_2_YELLOW = 4;
const int STATE_2_RED = 5;

// Light combinations for each state [light1_color, light2_color]
// where color index: 0=red, 1=yellow, 2=green
const int lightStates[6][2] = {
  {2, 0}, // STATE_1_GREEN:  Light1=Green, Light2=Red
  {1, 0}, // STATE_1_YELLOW: Light1=Yellow, Light2=Red
  {0, 0}, // STATE_1_RED:    Light1=Red, Light2=Red
  {0, 2}, // STATE_2_GREEN:  Light1=Red, Light2=Green
  {0, 1}, // STATE_2_YELLOW: Light1=Red, Light2=Yellow
  {0, 0}  // STATE_2_RED:    Light1=Red, Light2=Red
};

// Billboard messages
const char* messages[] = {
  "Drive Safely",
  "Buckle Up!",
  "Don't Text & Drive",
  "Speed Limit 50"
};
const int messageCount = 4;
const int messageChangeInterval = 5000; // 5 seconds per message

// Create LCD instance
LiquidCrystal lcd(lcdPins[0], lcdPins[1], lcdPins[2], lcdPins[3], lcdPins[4], lcdPins[5]);

// Speed calculation parameters
const float sensorDistance = 1.0; // Distance between photoresistors in meters
volatile unsigned long firstSensorTime = 0;
volatile unsigned long secondSensorTime = 0;
volatile bool speedMeasurementActive = false;
volatile bool displaySpeed = false;
float vehicleSpeed = 0.0;

// State tracking variables
int currentState = STATE_1_GREEN;
int currentMessage = 0;
unsigned long stateChangeTime = 0;
unsigned long messageChangeTime = 0;

// Function prototypes
void updateTrafficLights(int newState);
void setAllLightsRed();
void calculateSpeed();
void displayBillboardMessage();
void displaySpeedMessage();
void handleTrafficLights();
void updateDisplay();

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Traffic System");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  // Initialize traffic light pins
  for (int i = 0; i < 3; i++) {
    pinMode(trafficLight1[i], OUTPUT);
    pinMode(trafficLight2[i], OUTPUT);
  }

  // Initialize speed sensor pins with pull-up resistors
  pinMode(speedSensors[0], INPUT_PULLUP);
  pinMode(speedSensors[1], INPUT_PULLUP);

  // Attach interrupts for speed sensors
  attachInterrupt(digitalPinToInterrupt(speedSensors[0]), sensorOneTriggered, FALLING);
  attachInterrupt(digitalPinToInterrupt(speedSensors[1]), sensorTwoTriggered, FALLING);

  // Start with all traffic lights red
  setAllLightsRed();
  delay(2000);

  // Set initial state
  updateTrafficLights(STATE_1_GREEN);
  stateChangeTime = millis();
  messageChangeTime = millis();

  lcd.clear();
  displayBillboardMessage();
}

void loop() {
  // Handle traffic light state changes
  handleTrafficLights();

  // Display billboard message or speed
  updateDisplay();
}

void handleTrafficLights() {
  unsigned long currentTime = millis();

  // Check if it's time to change traffic light state
  if (currentState == STATE_1_GREEN && currentTime - stateChangeTime >= greenTime) {
    updateTrafficLights(STATE_1_YELLOW);
    stateChangeTime = currentTime;
  }
  else if (currentState == STATE_1_YELLOW && currentTime - stateChangeTime >= yellowTime) {
    updateTrafficLights(STATE_1_RED);
    stateChangeTime = currentTime;
  }
  else if (currentState == STATE_1_RED && currentTime - stateChangeTime >= allRedTime) {
    updateTrafficLights(STATE_2_GREEN);
    stateChangeTime = currentTime;
  }
  else if (currentState == STATE_2_GREEN && currentTime - stateChangeTime >= greenTime) {
    updateTrafficLights(STATE_2_YELLOW);
    stateChangeTime = currentTime;
  }
  else if (currentState == STATE_2_YELLOW && currentTime - stateChangeTime >= yellowTime) {
    updateTrafficLights(STATE_2_RED);
    stateChangeTime = currentTime;
  }
  else if (currentState == STATE_2_RED && currentTime - stateChangeTime >= allRedTime) {
    updateTrafficLights(STATE_1_GREEN);
    stateChangeTime = currentTime;
  }
}

void updateTrafficLights(int newState) {
  // Set all lights to off initially
  for (int i = 0; i < 3; i++) {
    digitalWrite(trafficLight1[i], LOW);
    digitalWrite(trafficLight2[i], LOW);
  }

  // Set appropriate lights based on the state array
  digitalWrite(trafficLight1[lightStates[newState][0]], HIGH);
  digitalWrite(trafficLight2[lightStates[newState][1]], HIGH);

  currentState = newState;
  Serial.print("Traffic light state changed to: ");
  Serial.println(newState);
}

void setAllLightsRed() {
  // Turn all lights off first
  for (int i = 0; i < 3; i++) {
    digitalWrite(trafficLight1[i], LOW);
    digitalWrite(trafficLight2[i], LOW);
  }
  
  // Turn on only red lights
  digitalWrite(trafficLight1[0], HIGH);  // Red light 1
  digitalWrite(trafficLight2[0], HIGH);  // Red light 2
}

void sensorOneTriggered() {
  firstSensorTime = millis();
  speedMeasurementActive = true;
  Serial.println("First sensor triggered");
}

void sensorTwoTriggered() {
  if (speedMeasurementActive) {
    secondSensorTime = millis();
    calculateSpeed();
    speedMeasurementActive = false;
    displaySpeed = true;
    Serial.print("Speed: ");
    Serial.print(vehicleSpeed);
    Serial.println(" km/h");
  }
}

void calculateSpeed() {
  unsigned long timeDifference = secondSensorTime - firstSensorTime;

  // Avoid division by zero
  if (timeDifference == 0) {
    vehicleSpeed = 0;
    return;
  }

  // Calculate speed in meters per second
  float speedMS = (sensorDistance * 1000.0) / timeDifference;

  // Convert to km/h
  vehicleSpeed = speedMS * 3.6;
}

void displayBillboardMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to City");
  lcd.setCursor(0, 1);
  lcd.print(messages[currentMessage]);
}

void displaySpeedMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vehicle Speed:");
  lcd.setCursor(0, 1);
  lcd.print(vehicleSpeed, 1);
  lcd.print(" km/h");
}

void updateDisplay() {
  unsigned long currentTime = millis();

  // If we need to display speed, show it for 5 seconds
  if (displaySpeed) {
    static unsigned long speedDisplayStartTime = 0;

    if (speedDisplayStartTime == 0) {
      displaySpeedMessage();
      speedDisplayStartTime = currentTime;
    }

    // After 5 seconds, go back to regular messages
    if (currentTime - speedDisplayStartTime >= 5000) {
      displaySpeed = false;
      speedDisplayStartTime = 0;
    }
  }
  // Otherwise show cycling billboard messages
  else {
    if (currentTime - messageChangeTime >= messageChangeInterval) {
      currentMessage = (currentMessage + 1) % messageCount;
      displayBillboardMessage();
      messageChangeTime = currentTime;
    }
  }
}