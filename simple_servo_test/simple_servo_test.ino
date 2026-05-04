#include <ESP32Servo.h>

Servo s1, s2, s3, s4, s5;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("========================================");
  Serial.println("  SERVO TEST - FULL RANGE");
  Serial.println("========================================");

  s1.attach(4, 500, 2500);
  s2.attach(5, 500, 2500);
  s3.attach(12, 500, 2500);
  s4.attach(13, 500, 2500);
  s5.attach(14, 500, 2500);

  Serial.println("Servos attached to GPIO: 4, 5, 12, 13, 14");
  delay(500);
}

void loop() {
  Serial.println(">>> Position 0 (FULL OPEN)");
  s1.write(0); s2.write(0); s3.write(0); s4.write(0); s5.write(0);
  delay(1500);

  Serial.println(">>> Position 90 (CENTER)");
  s1.write(90); s2.write(90); s3.write(90); s4.write(90); s5.write(90);
  delay(1500);

  Serial.println(">>> Position 180 (FULL CLOSE)");
  s1.write(180); s2.write(180); s3.write(180); s4.write(180); s5.write(180);
  delay(1500);

  Serial.println(">>> Position 90 (CENTER)");
  s1.write(90); s2.write(90); s3.write(90); s4.write(90); s5.write(90);
  delay(1500);
}
