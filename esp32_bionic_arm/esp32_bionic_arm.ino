/*
 * ESP32 Bionic Arm Controller - Direct Servo Control
 *
 * Controls 5 finger servos directly via GPIO pins (no PCA9685 driver)
 * Receives commands via Serial/UART from gesture detection system
 */

#include <ESP32Servo.h>

// ============================================
// SERVO GPIO PIN ASSIGNMENTS
// ============================================
#define SERVO_THUMB_PIN   4
#define SERVO_INDEX_PIN   5
#define SERVO_MIDDLE_PIN  12
#define SERVO_RING_PIN    13
#define SERVO_PINKY_PIN   14

// ============================================
// SERVO SETTINGS
// ============================================
#define SERVO_MIN 500
#define SERVO_MAX 2500
#define MOVE_DELAY 30    // Slower = smoother
#define MOVE_STEP 2      // Smaller steps = smoother

// ============================================
// SERVO OBJECTS
// ============================================
Servo thumbServo;
Servo indexServo;
Servo middleServo;
Servo ringServo;
Servo pinkyServo;

// Use pointers to avoid object copying issues
Servo* servos[] = {&thumbServo, &indexServo, &middleServo, &ringServo, &pinkyServo};
const int NUM_SERVOS = 5;

// ============================================
// GESTURE POSITIONS (0-180 degrees)
// ============================================
// Maximum range - servos will move as much as possible
const int rock_pos[] = {180, 180, 180, 180, 180};    // Full closed
const int paper_pos[] = {0, 0, 0, 0, 0};              // Full open
const int scissors_pos[] = {180, 0, 0, 180, 180};
const int point_pos[] = {180, 0, 180, 180, 180};
const int thumbsup_pos[] = {0, 180, 180, 180, 180};
const int ok_pos[] = {90, 90, 180, 180, 180};

// ============================================
// CURRENT STATE
// ============================================
int currentPositions[] = {90, 90, 90, 90, 90};

// ============================================
// SETUP
// ============================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("========================================");
  Serial.println("   ESP32 Bionic Arm Controller");
  Serial.println("========================================");

  // Initialize servos
  thumbServo.attach(SERVO_THUMB_PIN, SERVO_MIN, SERVO_MAX);
  indexServo.attach(SERVO_INDEX_PIN, SERVO_MIN, SERVO_MAX);
  middleServo.attach(SERVO_MIDDLE_PIN, SERVO_MIN, SERVO_MAX);
  ringServo.attach(SERVO_RING_PIN, SERVO_MIN, SERVO_MAX);
  pinkyServo.attach(SERVO_PINKY_PIN, SERVO_MIN, SERVO_MAX);

  Serial.println("Servos attached:");
  Serial.printf("  Thumb:  GPIO%d\n", SERVO_THUMB_PIN);
  Serial.printf("  Index:  GPIO%d\n", SERVO_INDEX_PIN);
  Serial.printf("  Middle: GPIO%d\n", SERVO_MIDDLE_PIN);
  Serial.printf("  Ring:   GPIO%d\n", SERVO_RING_PIN);
  Serial.printf("  Pinky:  GPIO%d\n", SERVO_PINKY_PIN);

  // Move to open position on startup
  Serial.println("Moving to open position...");
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i]->write(paper_pos[i]);
    currentPositions[i] = paper_pos[i];
  }
  delay(1000);

  Serial.println("========================================");
  Serial.println("  Ready! Commands: ROCK, PAPER, SCISSORS, CALIBRATE");
  Serial.println("========================================");
}

// ============================================
// SERVO CONTROL FUNCTIONS
// ============================================
void moveServosSmoothly(const int targetPositions[]) {
  bool allDone = false;

  while (!allDone) {
    allDone = true;

    for (int i = 0; i < NUM_SERVOS; i++) {
      int diff = targetPositions[i] - currentPositions[i];

      if (abs(diff) > MOVE_STEP) {
        currentPositions[i] += (diff > 0) ? MOVE_STEP : -MOVE_STEP;
        allDone = false;
      } else if (diff != 0) {
        currentPositions[i] = targetPositions[i];
      }

      servos[i]->write(currentPositions[i]);
    }

    delay(MOVE_DELAY);
  }
}

void moveToFist() {
  Serial.println("Moving to: FIST (ROCK)");
  moveServosSmoothly(rock_pos);
  Serial.println("OK");
}

void moveToOpen() {
  Serial.println("Moving to: OPEN (PAPER)");
  moveServosSmoothly(paper_pos);
  Serial.println("OK");
}

void moveToScissors() {
  Serial.println("Moving to: SCISSORS");
  moveServosSmoothly(scissors_pos);
  Serial.println("OK");
}

void moveToPoint() {
  Serial.println("Moving to: POINT");
  moveServosSmoothly(point_pos);
  Serial.println("OK");
}

void moveToThumbsUp() {
  Serial.println("Moving to: THUMBS UP");
  moveServosSmoothly(thumbsup_pos);
  Serial.println("OK");
}

void moveToOK() {
  Serial.println("Moving to: OK SIGN");
  moveServosSmoothly(ok_pos);
  Serial.println("OK");
}

void calibrateServos() {
  Serial.println("=== SERVO CALIBRATION MODE ===");

  for (int i = 0; i < NUM_SERVOS; i++) {
    Serial.printf("Testing servo %d on GPIO %d...\n", i,
      (i == 0) ? SERVO_THUMB_PIN :
      (i == 1) ? SERVO_INDEX_PIN :
      (i == 2) ? SERVO_MIDDLE_PIN :
      (i == 3) ? SERVO_RING_PIN : SERVO_PINKY_PIN);

    // Move to open
    servos[i]->write(10);
    delay(1000);

    // Move to center
    servos[i]->write(90);
    delay(1000);

    // Move to closed
    servos[i]->write(170);
    delay(1000);

    // Back to center
    servos[i]->write(90);
    delay(500);

    currentPositions[i] = 90;
  }

  Serial.println("Calibration complete!");
  Serial.println("OK");
}

// ============================================
// COMMAND PARSER
// ============================================
void processCommand(String command) {
  command.trim();
  command.toUpperCase();

  if (command == "ROCK" || command == "FIST") {
    moveToFist();
  } else if (command == "PAPER" || command == "OPEN") {
    moveToOpen();
  } else if (command == "SCISSORS") {
    moveToScissors();
  } else if (command == "POINT") {
    moveToPoint();
  } else if (command == "THUMBSUP") {
    moveToThumbsUp();
  } else if (command == "OK") {
    moveToOK();
  } else if (command == "CALIBRATE") {
    calibrateServos();
  } else if (command == "THUMB") {
    Serial.println("Testing THUMB servo...");
    thumbServo.write(20); delay(500); thumbServo.write(160); delay(500); thumbServo.write(90);
  } else if (command == "INDEX") {
    Serial.println("Testing INDEX servo...");
    indexServo.write(10); delay(500); indexServo.write(170); delay(500); indexServo.write(90);
  } else if (command == "MIDDLE") {
    Serial.println("Testing MIDDLE servo...");
    middleServo.write(10); delay(500); middleServo.write(170); delay(500); middleServo.write(90);
  } else if (command == "RING") {
    Serial.println("Testing RING servo...");
    ringServo.write(10); delay(500); ringServo.write(170); delay(500); ringServo.write(90);
  } else if (command == "PINKY") {
    Serial.println("Testing PINKY servo...");
    pinkyServo.write(10); delay(500); pinkyServo.write(170); delay(500); pinkyServo.write(90);
  } else if (command == "HELP") {
    Serial.println("Commands: ROCK, PAPER, SCISSORS, POINT, THUMBSUP, OK, CALIBRATE");
    Serial.println("Test individual servos: THUMB, INDEX, MIDDLE, RING, PINKY");
  } else if (command.length() > 0) {
    Serial.println("Unknown: " + command);
  }
}

// ============================================
// MAIN LOOP
// ============================================
void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    processCommand(command);
  }
  delay(10);
}
