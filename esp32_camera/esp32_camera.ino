/*
 * ESP32-CAM #1 - Camera & Game Logic
 *
 * Features:
 * - Streams MJPEG video via WiFi
 * - Communicates with ESP32 #2 (Servo) via UART
 * - Implements Rock Paper Scissors game logic
 * - Tracks and displays scores on video overlay
 *
 * UART Protocol:
 * - ESP32#1 -> ESP32#2: "PLAY\n" (request move)
 * - ESP32#2 -> ESP32#1: "MOVE:rock\n", "MOVE:paper\n", "MOVE:scissors\n"
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>

// ============================================
// WARNING: Disable brownout detector
// Only for ESP32-CAM due to power issues
// ============================================
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// ============================================
// WiFi Configuration (AP Mode)
// ============================================
const char* ap_ssid = "ESP32-CAM";
const char* ap_password = "12345678";

// ============================================
// UART Configuration for ESP32-CAM to ESP32-CAM
// ============================================
// Using UART1 for inter-ESP32-CAM communication
// UART0 is used for USB programming/debugging
#define UART_RX_PIN 33      // GPIO33 - RX from ESP32-CAM #2 (connect to TX of other ESP32-CAM)
#define UART_TX_PIN 4       // GPIO4 - TX to ESP32-CAM #2 (connect to RX of other ESP32-CAM)
#define UART_BAUD 115200

// HardwareSerial for UART1 (dedicated for ESP32-ESP32 communication)
HardwareSerial ESP32Serial(1);  // Use UART1

// ============================================
// Camera Pin Configuration for AI-Thinker ESP32-CAM
// ============================================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ============================================
// Web Server
// ============================================
WebServer server(80);

// ============================================
// Game State
// ============================================
struct GameState {
  int playerScore = 0;
  int esp32Score = 0;
  int tieScore = 0;
  String lastPlayerMove = "none";
  String lastEsp32Move = "none";
  String lastResult = "none";  // "player", "esp32", "tie", "unknown"
  unsigned long lastRoundTime = 0;
  bool waitingForEsp32 = false;
};

GameState game;

// ============================================
// Game Logic
// ============================================

// Winning combinations: key beats value
const String WINNING_COMBOS[3][2] = {
  {"rock", "scissors"},    // Rock crushes scissors
  {"scissors", "paper"},   // Scissors cuts paper
  {"paper", "rock"}        // Paper covers rock
};

String determineWinner(String playerMove, String esp32Move) {
  // Normalize to lowercase
  playerMove.toLowerCase();
  esp32Move.toLowerCase();

  // Check for valid moves
  if (playerMove != "rock" && playerMove != "paper" && playerMove != "scissors") {
    return "unknown";
  }
  if (esp32Move != "rock" && esp32Move != "paper" && esp32Move != "scissors") {
    return "unknown";
  }

  // Check for tie
  if (playerMove == esp32Move) {
    return "tie";
  }

  // Check if player wins
  for (int i = 0; i < 3; i++) {
    if (WINNING_COMBOS[i][0] == playerMove && WINNING_COMBOS[i][1] == esp32Move) {
      return "player";
    }
  }

  // Otherwise ESP32 wins
  return "esp32";
}

void updateScores(String result) {
  if (result == "player") {
    game.playerScore++;
  } else if (result == "esp32") {
    game.esp32Score++;
  } else if (result == "tie") {
    game.tieScore++;
  }
}

String getResultMessage(String result, String playerMove, String esp32Move) {
  if (result == "unknown") {
    return "Unable to determine winner";
  } else if (result == "tie") {
    return "It's a tie!";
  } else if (result == "player") {
    return playerMove + " beats " + esp32Move + "! You win!";
  } else {
    return esp32Move + " beats " + playerMove + "! ESP32 wins!";
  }
}

// ============================================
// UART Communication with ESP32 #2
// ============================================

bool sendPlayCommand() {
  // Send PLAY command to ESP32 #2
  ESP32Serial.println("PLAY");
  Serial.println("UART: Sent 'PLAY' command to ESP32 #2");
  return true;
}

String receiveEsp32Move(unsigned long timeout = 5000) {
  unsigned long startTime = millis();
  String received = "";

  Serial.println("UART: Waiting for ESP32 #2 response...");

  while (millis() - startTime < timeout) {
    if (ESP32Serial.available()) {
      char c = ESP32Serial.read();
      received += c;

      // Check for complete message (ends with newline)
      if (c == '\n') {
        received.trim();  // Remove whitespace including newline

        // Parse the response
        if (received.startsWith("MOVE:")) {
          String move = received.substring(5);  // Extract after "MOVE:"
          move.toLowerCase();
          Serial.println("UART: Received move: " + move);
          return move;
        } else {
          Serial.println("UART: Invalid response format: " + received);
          received = "";  // Reset and try again
        }
      }
    }
    delay(10);
  }

  Serial.println("UART: Timeout waiting for ESP32 #2");
  return "error";
}

// ============================================
// HTTP Endpoints
// ============================================

void handleStream() {
  // MJPEG streaming endpoint
  camera_fb_t *fb = NULL;

  // Send boundary header
  String head = "--frame\r\n";
  head += "Content-Type: image/jpeg\r\n\r\n";

  server.sendContent(head);

  // Capture and send frame
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    server.sendContent("--frame\r\n");
    return;
  }

  // Send the frame
  server.sendContent((const char *)fb->buf, fb->len);
  server.sendContent("\r\n");

  // Return framebuffer
  esp_camera_fb_return(fb);
}

void handlePlayRound() {
  // Get player move from query parameter
  if (!server.hasArg("move")) {
    server.send(400, "application/json", "{\"error\":\"Missing move parameter\"}");
    return;
  }

  String playerMove = server.arg("move");
  playerMove.toLowerCase();

  // Validate player move
  if (playerMove != "rock" && playerMove != "paper" && playerMove != "scissors") {
    server.send(400, "application/json", "{\"error\":\"Invalid move\"}");
    return;
  }

  Serial.println("=== NEW ROUND ===");
  Serial.println("Player move: " + playerMove);

  // Send PLAY command to ESP32 #2
  game.waitingForEsp32 = true;
  if (!sendPlayCommand()) {
    game.waitingForEsp32 = false;
    server.send(500, "application/json", "{\"error\":\"UART communication failed\"}");
    return;
  }

  // Wait for ESP32 #2 response
  String esp32Move = receiveEsp32Move();
  game.waitingForEsp32 = false;

  // Handle timeout/error
  if (esp32Move == "error") {
    // Fallback: generate random move locally
    int randomMove = random(3);
    if (randomMove == 0) esp32Move = "rock";
    else if (randomMove == 1) esp32Move = "paper";
    else esp32Move = "scissors";
    Serial.println("Using local random move (ESP32 timeout)");
  }

  Serial.println("ESP32 #2 move: " + esp32Move);

  // Determine winner
  String result = determineWinner(playerMove, esp32Move);

  // Update scores
  updateScores(result);

  // Update game state
  game.lastPlayerMove = playerMove;
  game.lastEsp32Move = esp32Move;
  game.lastResult = result;
  game.lastRoundTime = millis();

  // Generate message
  String message = getResultMessage(result, playerMove, esp32Move);
  Serial.println("Result: " + result);
  Serial.println("Message: " + message);
  Serial.println("Scores - Player: " + String(game.playerScore) +
                 " ESP32: " + String(game.esp32Score) +
                 " Tie: " + String(game.tieScore));
  Serial.println("==================");

  // Build JSON response
  String json = "{";
  json += "\"player_move\":\"" + playerMove + "\",";
  json += "\"esp32_move\":\"" + esp32Move + "\",";
  json += "\"result\":\"" + result + "\",";
  json += "\"message\":\"" + message + "\",";
  json += "\"scores\":{";
  json += "\"player\":" + String(game.playerScore) + ",";
  json += "\"esp32\":" + String(game.esp32Score) + ",";
  json += "\"tie\":" + String(game.tieScore);
  json += "},";
  json += "\"esp32_connected\":true";
  json += "}";

  server.send(200, "application/json", json);
}

void handleScores() {
  String json = "{";
  json += "\"player\":" + String(game.playerScore) + ",";
  json += "\"esp32\":" + String(game.esp32Score) + ",";
  json += "\"tie\":" + String(game.tieScore);
  json += "}";
  server.send(200, "application/json", json);
}

void handleReset() {
  game.playerScore = 0;
  game.esp32Score = 0;
  game.tieScore = 0;
  game.lastPlayerMove = "none";
  game.lastEsp32Move = "none";
  game.lastResult = "none";
  Serial.println("Scores reset");
  server.send(200, "application/json", "{\"message\":\"Scores reset\"}");
}

void handleStatus() {
  String json = "{";
  json += "\"status\":\"ready\",";
  json += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
  json += "\"last_player_move\":\"" + game.lastPlayerMove + "\",";
  json += "\"last_esp32_move\":\"" + game.lastEsp32Move + "\",";
  json += "\"last_result\":\"" + game.lastResult + "\",";
  json += "\"waiting_for_esp32\":" + String(game.waitingForEsp32 ? "true" : "false") + ",";
  json += "\"uart_connected\":true";
  json += "}";
  server.send(200, "application/json", json);
}

void handleRoot() {
  String html = "<html><head><title>ESP32-CAM Game Server</title>";
  html += "<style>body{font-family:sans-serif;margin:40px;}";
  html += ".status{background:#f0f0f0;padding:20px;border-radius:10px;}";
  html += ".score{font-size:24px;font-weight:bold;}</style>";
  html += "</head><body>";
  html += "<h1>ESP32-CAM Rock Paper Scissors</h1>";
  html += "<div class='status'>";
  html += "<p>Status: <strong>Ready</strong></p>";
  html += "<p>IP: <strong>" + WiFi.softAPIP().toString() + "</strong></p>";
  html += "<hr>";
  html += "<h2>Scores</h2>";
  html += "<p class='score'>Player: " + String(game.playerScore) + " | ";
  html += "ESP32: " + String(game.esp32Score) + " | ";
  html += "Tie: " + String(game.tieScore) + "</p>";
  html += "<hr>";
  html += "<h2>API Endpoints</h2>";
  html += "<ul>";
  html += "<li>GET /stream - MJPEG video stream</li>";
  html += "<li>POST /play?move=rock - Play a round</li>";
  html += "<li>GET /scores - Get current scores</li>";
  html += "<li>POST /reset - Reset scores</li>";
  html += "<li>GET /status - Get status</li>";
  html += "</ul>";
  html += "</div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// ============================================
// Camera Setup
// ============================================

bool setupCamera() {
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Lower resolution for better performance
  config.frame_size = FRAMESIZE_QVGA;  // 320x240
  config.jpeg_quality = 12;
  config.fb_count = 2;

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }

  // Adjust sensor settings
  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV2640_PID) {
    s->set_brightness(s, 0);     // -2 to 2
    s->set_contrast(s, 0);       // -2 to 2
    s->set_saturation(s, 0);     // -2 to 2
  }

  Serial.println("Camera initialized successfully");
  return true;
}

// ============================================
// Setup
// ============================================

void setup() {
  // Disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // Initialize Serial for debugging
  Serial.begin(115200);
  delay(2000);

  Serial.println("========================================");
  Serial.println("   ESP32-CAM #1 - Camera & Game Logic");
  Serial.println("========================================");

  // Initialize UART1 for ESP32 communication
  ESP32Serial.begin(UART_BAUD, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
  Serial.println("UART1 initialized on RX=" + String(UART_RX_PIN) + " TX=" + String(UART_TX_PIN));

  // Initialize random seed
  randomSeed(analogRead(34) + millis());

  // Setup Camera
  Serial.println("Initializing camera...");
  if (!setupCamera()) {
    Serial.println("Camera setup failed!");
    delay(10000);
    ESP.restart();
  }

  // Setup WiFi AP
  Serial.println("Setting up WiFi AP...");
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Setup web server routes
  server.on(HTTP_GET, "/", handleRoot);
  server.on(HTTP_GET, "/stream", handleStream);
  server.on(HTTP_POST, "/play", handlePlayRound);
  server.on(HTTP_GET, "/scores", handleScores);
  server.on(HTTP_POST, "/reset", handleReset);
  server.on(HTTP_GET, "/status", handleStatus);

  server.begin();
  Serial.println("HTTP server started");

  Serial.println("========================================");
  Serial.println("  ESP32-CAM #1 Ready!");
  Serial.println("========================================");
  Serial.println("Connect to: " + String(ap_ssid));
  Serial.println("Password: " + String(ap_password));
  Serial.println("Open browser: http://" + IP.toString());
  Serial.println("========================================");
  Serial.println();

  // Clear any existing UART data
  while (ESP32Serial.available()) {
    ESP32Serial.read();
  }
}

// ============================================
// Main Loop
// ============================================

void loop() {
  // Handle HTTP requests
  server.handleClient();

  // Handle incoming UART messages (for debugging/testing)
  if (ESP32Serial.available()) {
    String msg = ESP32Serial.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) {
      Serial.print("UART Received: ");
      Serial.println(msg);
    }
  }

  delay(2);
}
