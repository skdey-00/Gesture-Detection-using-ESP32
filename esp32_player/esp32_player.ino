/*
 * ESP32 Player - Rock Paper Scissors Opponent
 */

#include <WiFi.h>
#include <WebServer.h>

// Disable brownout detector (WARNING: only for testing!)
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// WiFi Settings
const char* ssid = "ESP32-CAM";
const char* password = "12345678";

// Web server on port 81
WebServer server(81);

// Game variables
String lastMove = "none";
int totalMoves = 0;

void setup() {
  // Disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  delay(2000);

  Serial.println("========================================");
  Serial.println("   ESP32 Rock Paper Scissors Player");
  Serial.println("========================================");
  Serial.println("Setup starting...");

  // Initialize random seed
  randomSeed(analogRead(34));
  Serial.println("Random seed initialized");

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);  // Disable WiFi sleep for better performance
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed!");
  }

  // Setup server routes
  server.on("/", []() {
    server.send(200, "text/html", "<html><body><h1>ESP32 Player Ready</h1><p>IP: " + WiFi.localIP().toString() + "</p><p>Use /move endpoint</p></body></html>");
  });

  server.on("/move", []() {
    const char* moves[] = {"rock", "paper", "scissors"};
    lastMove = moves[random(3)];
    totalMoves++;

    String json = "{\"move\":\"" + lastMove + "\",\"total_moves\":" + String(totalMoves) + "}";
    Serial.println("Move: " + lastMove);
    server.send(200, "application/json", json);
  });

  server.on("/status", []() {
    String json = "{\"status\":\"ready\",";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"last_move\":\"" + lastMove + "\",";
    json += "\"total_moves\":" + String(totalMoves) + "}";
    server.send(200, "application/json", json);
  });

  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("HTTP server started on port 81");
  Serial.println("========================================");
  Serial.println("  ESP32 Player Ready!");
  Serial.println("========================================");
  Serial.println();
}

void loop() {
  server.handleClient();
  delay(2);
}
