#include <ESP32Servo.h>

Servo testServo;

void setup() {
  Serial.begin(115200);

  Serial.println("Starting servo test...");

  // Try GPIO 4 first (works on most ESP32 boards)
  testServo.attach(4, 500, 2500);

  Serial.println("Servo attached to GPIO 4");
  Serial.println("Moving servo...");
}

void loop() {
  Serial.println("Position: 0 degrees");
  testServo.write(0);
  delay(1000);

  Serial.println("Position: 90 degrees");
  testServo.write(90);
  delay(1000);

  Serial.println("Position: 180 degrees");
  testServo.write(180);
  delay(1000);

  Serial.println("Position: 90 degrees");
  testServo.write(90);
  delay(1000);
}
