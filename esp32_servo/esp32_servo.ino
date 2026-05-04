/*
 * ESP32-CAM #2 - Servo Controller with PCA9685
 *
 * Designed for ESP32-CAM (AI-Thinker) with PCA9685 16-Channel PWM/Servo Driver
 *
 * Features:
 * - Receives PLAY command from ESP32-CAM #1 via UART
 * - Randomly selects Rock, Paper, or Scissors
 * - Controls servos via PCA9685 (I2C)
 * - Sends selected move back to ESP32-CAM #1 via UART
 *
 * UART Protocol:
 * - ESP32#1 -> ESP32#2: "PLAY\n" (request move)
 * - ESP32#2 -> ESP32#1: "MOVE:rock\n", "MOVE:paper\n", "MOVE:scissors\n"
 *
 * I2C Protocol (PCA9685):
 * - SDA: GPIO13
 * - SCL: GPIO12
 * - Default PCA9685 address: 0x40
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Disable brownout detector (ESP32-CAM specific)
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// ============================================
// PCA9685 Servo Driver Settings
// ============================================
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// For ESP32-CAM, use Wire1 for I2C to avoid camera I2C conflict
// Wire1 uses GPIO13 (SDA) and GPIO12 (SCL)
#define I2C_SDA_PIN 13
#define I2C_SCL_PIN 12

// Servo settings
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz updates

// Pulse length range for servos (in units of 0.5us)
// PCA9685 uses 0-4095 where each tick = 0.5us
// So: 150 = 75us (too short), 1500 = 750us (too short)
// Actually: SERVOMIN = 150 (0.5us * 150 = 75us) - this is wrong
// Let me recalculate: For 1ms to 2ms pulse:
// 1ms = 1000us = 2000 ticks of 0.5us
// 2ms = 2000us = 4000 ticks of 0.5us
#define SERVOMIN  150  // This is 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600  // This is 'maximum' pulse length count (out of 4096)

// Alternative: Use microsecond values directly
#define US_MIN  500   // 0.5ms
#define US_MAX  2500  // 2.5ms
#define US_MID  1500  // 1.5ms (center)

// Convert microseconds to PCA9685 ticks (0.5us per tick)
inline int usToTicks(int us) {
  return us / 2;  // Each tick = 0.5us
}

// ============================================
// UART Configuration for ESP32-CAM to ESP32-CAM
// ============================================
#define CAM_UART_TX_PIN  4    // GPIO4 - TX to other ESP32-CAM
#define CAM_UART_RX_PIN  33   // GPIO33 - RX from other ESP32-CAM
#define UART_BAUD 115200

HardwareSerial ESP32Comm(1);  // UART1 for ESP32-CAM communication

// ============================================
// Servo Channel Mapping on PCA9685
// ============================================
// The PCA9685 has 16 channels (0-15)
// Map each finger to a channel
#define SERVO_THUMB    0
#define SERVO_INDEX    1
#define SERVO_MIDDLE   2
#define SERVO_RING     3
#define SERVO_PINKY    4
#define SERVO_WRIST    5    // Optional

// ============================================
// Gesture Positions (ADJUST FOR YOUR HARDWARE)
// ============================================
// Values are in microseconds (500-2500 range)

// ROCK: All fingers closed (fist)
const int rock_pos[] = {
  2400,  // Thumb - closed
  2300,  // Index - closed
  2300,  // Middle - closed
  2300,  // Ring - closed
  2300,  // Pinky - closed
  1500   // Wrist - neutral
};

// PAPER: All fingers open
const int paper_pos[] = {
  600,   // Thumb - open
  700,   // Index - open
  700,   // Middle - open
  700,   // Ring - open
  700,   // Pinky - open
  1500   // Wrist - neutral
};

// SCISSORS: Index and middle extended, others closed
const int scissors_pos[] = {
  600,   // Thumb - closed (or open for traditional scissors)
  700,   // Index - open/extended
  700,   // Middle - open/extended
  2300,  // Ring - closed
  2300,  // Pinky - closed
  1500   // Wrist - neutral
};

const int NUM_SERVOS = 6;

// ============================================
// Game State
// ============================================
String currentMove = "none";
int totalMoves = 0;
bool moveInProgress = false;

// ============================================
// PCA9685 Servo Control Functions
// ============================================

void initPCA9685() {
  Serial.println("Initializing PCA9685...");

  // Initialize Wire1 with custom pins for ESP32-CAM
  Wire1.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // Initialize PCA9685
  pwm.begin();

  // Set servo frequency (analog servos run at ~50 Hz)
  pwm.setPWMFreq(SERVO_FREQ);

  Serial.println("PCA9685 initialized!");
}

void setServoUs(uint8_t channel, int us) {
  // Convert microseconds to PWM pulse length
  // PCA9685 pulse length is 0-4095, where each tick = 0.5us
  int pulseLength = constrain(us, US_MIN, US_MAX) / 2;
  pwm.setPWM(channel, 0, pulseLength);
}

void setServoDegrees(uint8_t channel, int degrees) {
  // Convert degrees to microseconds
  degrees = constrain(degrees, 0, 180);
  int us = map(degrees, 0, 180, US_MIN, US_MAX);
  setServoUs(channel, us);
}

void moveServosSmoothly(const int* targetPositions, int delayMs = 15) {
  // Get current positions
  static int currentPositions[NUM_SERVOS];
  static bool initialized = false;

  if (!initialized) {
    // Start from mid position on first run
    for (int i = 0; i < NUM_SERVOS; i++) {
      currentPositions[i] = US_MID;
    }
    initialized = true;
  }

  // Move each servo incrementally
  bool allDone = false;
  int step = 20;  // Microseconds per step (adjust for speed)

  while (!allDone) {
    allDone = true;

    for (int i = 0; i < NUM_SERVOS; i++) {
      int diff = targetPositions[i] - currentPositions[i];

      if (abs(diff) > step) {
        currentPositions[i] += (diff > 0) ? step : -step;
        allDone = false;
      } else if (diff != 0) {
        currentPositions[i] = targetPositions[i];
      }

      // Map servo index to PCA9685 channel
      uint8_t channel = i;
      setServoUs(channel, currentPositions[i]);
    }

    delay(delayMs);
  }
}

void showGesture(String gesture) {
  Serial.println("Moving servos to show: " + gesture);

  if (gesture == "rock") {
    moveServosSmoothly(rock_pos);
  } else if (gesture == "paper") {
    moveServosSmoothly(paper_pos);
  } else if (gesture == "scissors") {
    moveServosSmoothly(scissors_pos);
  } else {
    Serial.println("Unknown gesture, moving to neutral");
    // Move to neutral position
    const int neutral_pos[] = {US_MID, US_MID, US_MID, US_MID, US_MID, US_MID};
    moveServosSmoothly(neutral_pos);
  }
}

// ============================================
// Game Logic
// ============================================

String generateRandomMove() {
  int randomChoice = random(3);

  switch (randomChoice) {
    case 0:
      return "rock";
    case 1:
      return "paper";
    case 2:
      return "scissors";
    default:
      return "rock";
  }
}

void playRound() {
  if (moveInProgress) {
    Serial.println("Move already in progress, ignoring");
    return;
  }

  moveInProgress = true;
  totalMoves++;

  // Generate random move
  currentMove = generateRandomMove();
  Serial.print("Selected move: ");
  Serial.println(currentMove);

  // Physically show the gesture with servos
  showGesture(currentMove);

  // Send response back to ESP32 #1
  String response = "MOVE:" + currentMove + "\n";
  ESP32Comm.print(response);
  Serial.print("UART Response sent: ");
  Serial.println(response);

  moveInProgress = false;
}

// ============================================
// Setup
// ============================================

void setup() {
  // Disable brownout detector (ESP32-CAM specific)
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // Initialize Serial for debugging
  Serial.begin(115200);
  delay(2000);

  Serial.println("========================================");
  Serial.println("   ESP32-CAM #2 - Servo Controller");
  Serial.println("   with PCA9685 Driver");
  Serial.println("========================================");

  // Initialize PCA9685
  initPCA9685();

  // Initialize UART for ESP32-CAM communication
  // UART1: TX=GPIO4, RX=GPIO33
  ESP32Comm.begin(UART_BAUD, SERIAL_8N1, CAM_UART_RX_PIN, CAM_UART_TX_PIN);
  Serial.println("UART1 initialized on RX=GPIO33 TX=GPIO4");

  // Initialize random seed
  randomSeed(analogRead(34) + millis());
  Serial.println("Random seed initialized");

  // Move servos to neutral position on startup
  Serial.println("Moving servos to neutral position...");
  const int neutral_pos[] = {US_MID, US_MID, US_MID, US_MID, US_MID, US_MID};
  moveServosSmoothly(neutral_pos, 20);

  Serial.println("========================================");
  Serial.println("  ESP32-CAM Servo Controller Ready!");
  Serial.println("========================================");
  Serial.println("PCA9685 Channels:");
  Serial.println("  Ch0: Thumb");
  Serial.println("  Ch1: Index");
  Serial.println("  Ch2: Middle");
  Serial.println("  Ch3: Ring");
  Serial.println("  Ch4: Pinky");
  Serial.println("  Ch5: Wrist");
  Serial.println("Waiting for PLAY command from ESP32 #1");
  Serial.println();

  // Clear any existing UART data
  while (ESP32Comm.available()) {
    ESP32Comm.read();
  }
}

// ============================================
// Main Loop
// ============================================

void loop() {
  // Check for incoming UART commands from ESP32 #1
  if (ESP32Comm.available()) {
    String command = ESP32Comm.readStringUntil('\n');
    command.trim();  // Remove whitespace including newline

    if (command.length() > 0) {
      Serial.print("UART Received: ");
      Serial.println(command);

      // Check for PLAY command
      if (command == "PLAY") {
        Serial.println("PLAY command received, starting round...");
        playRound();
      } else {
        Serial.println("Unknown command: " + command);
      }
    }
  }

  delay(10);
}
