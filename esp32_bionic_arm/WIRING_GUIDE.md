# ESP32 Bionic Arm - Wiring Guide

## Hardware Required

| Component | Quantity | Notes |
|-----------|----------|-------|
| ESP32 Board | 1 | Regular ESP32 dev board (NOT ESP32-CAM) |
| SG90 or MG90S Servo | 5 | One for each finger |
| 5V Power Supply | 1 | 2A minimum for all servos |
| Jumper Wires | - | Male-to-female and male-to-male |
| Breadboard or PCB | 1 | For connections |

---

## ESP32 Pin Assignment

| Finger | ESP32 GPIO | Servo Wire Color |
|--------|------------|------------------|
| Thumb | GPIO 18 | Orange/Yellow (signal) |
| Index | GPIO 19 | Orange/Yellow (signal) |
| Middle | GPIO 21 | Orange/Yellow (signal) |
| Ring | GPIO 22 | Orange/Yellow (signal) |
| Pinky | GPIO 23 | Orange/Yellow (signal) |

**Why these pins?** These are safe GPIOs that won't interfere with ESP32 boot.

---

## Servo Wire Colors

| Wire Color | Function | Connection |
|------------|----------|------------|
| **Red (or brown on some)** | VCC / Power | +5V from external supply |
| **Brown/Black** | GND / Ground | Common ground with ESP32 |
| **Orange/Yellow** | Signal | ESP32 GPIO pin |

---

## Wiring Diagram

```
                    EXTERNAL 5V POWER SUPPLY (2A+)
                    ─────────────────────────────
                               │
                    ┌──────────┴──────────┐
                    │                     │
                   5V                    GND
                    │                     │
        ┌───────────┼─────────────────────┼────────────┐
        │           │                     │            │
     ┌──┴──┐     ┌──┴──┐               ┌──┴──┐      │
     │SERVO│     │SERVO│               │ESP32│      │
     │ 1   │     │ 2-5 │               │     │      │
     └──┬──┘     └──┬──┘               └─┬───┘      │
        │           │                    │          │
       GND         GND                  GND─────────┘
        │           │                    │
        └───────────┴────────────────────┘
              COMMON GROUND CONNECTION
```

### Complete Wiring Table

| Servo | Signal (→ESP32) | VCC (→5V) | GND (→Common) |
|-------|-----------------|-----------|---------------|
| Thumb | GPIO 18 | +5V | GND |
| Index | GPIO 19 | +5V | GND |
| Middle | GPIO 21 | +5V | GND |
| Ring | GPIO 22 | +5V | GND |
| Pinky | GPIO 23 | +5V | GND |

### ESP32 Power
| ESP32 Pin | Connection |
|-----------|------------|
| VIN | 5V from USB or external supply |
| GND | Common ground with servos |
| 3.3V | Not used for servos |

---

## IMPORTANT: Use External Power for Servos!

**DO NOT power servos from ESP32's 5V pin!**

Each servo can draw up to 500mA when moving. With 5 servos:
- Peak current: ~2.5A
- ESP32 5V pin can only supply ~500mA total

**Result without external power:** ESP32 will brownout/reset when servos move.

### Recommended Power Setup
```
Wall Adapter (5V, 2A+) ───→ Servo VCC (all 5 red wires)
                          │
ESP32 GND ◄───────────────┴─────◄─── Servo GND (all brown wires)
```

---

## ESP32 Pin Mapping Visual

```
                    ESP32 Dev Board
                   ┌────────────────┐
                   │                │
              3.3V │ ▄             ▄ │ GND
         (Sensor)  │ █    ESP32    █ │ (Common)
       12-13 (MOSI) │ █   Dev Board █ │ 23 (Pinky)
              27 (MISO) │ █           █ │ 22 (Ring)
              14 (SCK) │ █           █ │ TX0
              GND │ █           █ │ RX0
              15 │ ▀           ▀ │ 21 (Middle)
                   │                │ 19 (Index)
                   │       USB       │ 18 (Thumb)
                   └────────────────┘
```

---

## Alternative GPIO Options

If pins 18-23 conflict with your setup, use these alternatives:

| Safe GPIOs | Avoid (Boot Strapping) |
|------------|------------------------|
| 4, 5, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33 | 0, 2, 12, 15 |

**Don't use GPIO 0, 2, 12, 15** - these affect boot mode.

---

## Testing the Setup

### 1. Power Test (No servos connected yet)
1. Upload the code to ESP32
2. Open Serial Monitor (115200 baud)
3. Should see "Bionic Arm Ready!" message

### 2. Single Servo Test
1. Connect ONE servo to Thumb pin (GPIO 18)
2. Send command: `CALIBRATE`
3. Servo should sweep through its range
4. If servo jitters or doesn't move, check power

### 3. All Servos Test
1. Connect remaining servos
2. Send command: `SCISSORS`
3. Index and middle should open, others closed
4. Send command: `FIST`
5. All fingers should close

---

## UART Communication (Optional)

If using with another device (Raspberry Pi, another ESP32, etc.):

```
ESP32 (Bionic Arm)          Controller
─────────────────          ──────────────
TX0 (GPIO1)    ───────────→ RX
RX0 (GPIO3)    ←─────────── TX
GND            ──────────── GND (IMPORTANT!)
```

**Baud Rate:** 115200

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Servos jittering | Use external 5V power supply |
| ESP32 resets when servos move | External power not connected or too weak |
| Servo not moving | Check signal wire, verify GPIO pin number |
| Wrong direction | Swap servo positions or adjust values in code (180-open/0-closed) |
| Only some servos work | Check individual connections, common ground |

---

## Calibration

Adjust these values in `esp32_bionic_arm.ino` for your hand mechanism:

```cpp
// 0 = fully open, 180 = fully closed
const int rock_pos[] = {
  160,   // Thumb - ADJUST IF NEEDED
  170,   // Index - ADJUST IF NEEDED
  170,   // Middle - ADJUST IF NEEDED
  170,   // Ring - ADJUST IF NEEDED
  170    // Pinky - ADJUST IF NEEDED
};
```

Use the `CALIBRATE` command to test each servo individually.
