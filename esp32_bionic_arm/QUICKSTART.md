# ESP32 Bionic Arm - Quick Start Guide

## 1. Arduino IDE Setup

### Install ESP32 Board Support
1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add to "Additional Boards Manager URLs":
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search "esp32" and install **ESP32 by Espressif Systems**

### Install ESP32Servo Library
1. Go to **Tools → Manage Libraries**
2. Search "ESP32Servo"
3. Install **ESP32Servo by Kevin Harrington**

---

## 2. Upload Code

1. Open `esp32_bionic_arm.ino`
2. Select Board: **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
3. Select Port: **Tools → Port → COM#** (your ESP32's port)
4. Click **Upload** button

---

## 3. Wire the Servos

Follow the pinout in `WIRING_GUIDE.md`:

| Servo | Signal Pin |
|-------|------------|
| Thumb | GPIO 18 |
| Index | GPIO 19 |
| Middle | GPIO 21 |
| Ring | GPIO 22 |
| Pinky | GPIO 23 |

**Remember:** Use external 5V power for servos!

---

## 4. Test

1. Open Serial Monitor (115200 baud)
2. You should see: "Bionic Arm Ready!"
3. Type commands and click Send:

```
ROCK       → Make fist
PAPER      → Open hand
SCISSORS   → Index + middle out
POINT      → Point with index
THUMBSUP   → Thumbs up
OK         → OK sign
CALIBRATE  → Test each servo
```

---

## 5. Integrate with Python (Optional)

If using with your gesture detection Python server:

```python
import serial

# Connect to ESP32
esp32 = serial.Serial('COM3', 115200)  # Change COM port as needed

# Send gesture command
esp32.write(b'ROCK\n')
response = esp32.readline().decode().strip()
print(response)  # Should print "OK"

# Close when done
esp32.close()
```

---

## Available Commands

| Command | Gesture |
|---------|---------|
| `ROCK` / `FIST` | All fingers closed |
| `PAPER` / `OPEN` | All fingers open |
| `SCISSORS` | Index & middle extended |
| `POINT` | Index finger only |
| `THUMBSUP` | Thumb up, others closed |
| `OK` | Thumb + index circle |
| `CALIBRATE` | Test each servo |
| `HELP` | Show command list |

---

## Adjusting for Your Hand

If servos move too far or not enough, edit these lines in the code:

```cpp
// For ROCK/fist (all closed)
const int rock_pos[] = {160, 170, 170, 170, 170};

// For PAPER/open (all open)
const int paper_pos[] = {20, 10, 10, 10, 10};
```

- Lower values = more open
- Higher values = more closed
- Range: 0-180 degrees
