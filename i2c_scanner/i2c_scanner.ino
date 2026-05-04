/*
 * I2C Scanner for ESP32-CAM
 *
 * Use this to verify:
 * 1. PCA9685 is connected correctly
 * 2. PCA9685 I2C address (should be 0x40 by default)
 *
 * Instructions:
 * 1. Upload this to ESP32-CAM #2
 * 2. Open Serial Monitor (115200 baud)
 * 3. Look for "Found device at 0x40"
 */

#include <Wire.h>

// Disable brownout detector (ESP32-CAM specific)
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// For ESP32-CAM, use Wire1 with custom pins
#define I2C_SDA_PIN 13
#define I2C_SCL_PIN 12

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  delay(2000);

  Serial.println("========================================");
  Serial.println("   I2C Scanner for ESP32-CAM");
  Serial.println("========================================");
  Serial.print("SDA: GPIO");
  Serial.println(I2C_SDA_PIN);
  Serial.print("SCL: GPIO");
  Serial.println(I2C_SCL_PIN);
  Serial.println();

  // Initialize Wire1
  Wire1.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  Serial.println("Scanning I2C bus...");
  Serial.println();

  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();

    if (error == 0) {
      Serial.print("✓ Found I2C device at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);

      if (address == 0x40) {
        Serial.println("  → This is the PCA9685! (default address)");
      }

      nDevices++;
    } else if (error == 4) {
      Serial.print("⚠ Unknown error at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  Serial.println();
  Serial.println("========================================");

  if (nDevices == 0) {
    Serial.println("❌ No I2C devices found!");
    Serial.println();
    Serial.println("Troubleshooting:");
    Serial.println("1. Check SDA/SCL connections");
    Serial.println("2. Check VCC (3.3V) and GND");
    Serial.println("3. Verify PCA9685 has power");
    Serial.println("4. Check if address jumpers are set");
  } else {
    Serial.print("✓ Found ");
    Serial.print(nDevices);
    Serial.println(" device(s)!");
  }

  Serial.println("========================================");
}

void loop() {
  // Do nothing, just scan once
  delay(5000);
}
