#include <ESP32Servo.h>

Servo indexServo;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Index Finger Test - GPIO 5");
  indexServo.attach(5);

  Serial.println("Ready!");
}

void loop() {
  Serial.println("OPEN (10 degrees)");
  indexServo.write(10);
  delay(1500);

  Serial.println("CLOSED (170 degrees)");
  indexServo.write(170);
  delay(1500);
}
