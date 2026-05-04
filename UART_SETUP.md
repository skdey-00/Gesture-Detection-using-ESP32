# UART-Based Rock Paper Scissors - Setup Guide (Dual ESP32-CAM)

## System Overview

This system uses **two ESP32-CAM modules** connected via UART (serial communication):

1. **ESP32-CAM #1 (Camera)** - that:
   - Streams video via WiFi to Python server for gesture detection
   - Implements game logic and score tracking
   - Communicates with ESP32-CAM #2 via UART
   - Controls when the opponent plays

2. **ESP32-CAM #2 (Servo)** - that:
   - Receives commands via UART from ESP32-CAM #1
   - Randomly selects Rock, Paper, or Scissors
   - Controls servos to physically display the gesture
   - Sends move back to ESP32-CAM #1

## Hardware Required

### Both Units
- 2x ESP32-CAM modules (AI-Thinker model recommended)
- 2x USB to Serial adapters for programming
- 2x 5V power supplies (ESP32-CAM can be power-hungry, especially with servos)

### For ESP32-CAM #2 (Servo Unit)
- **Servos** (2-6 depending on your hand design):
  - Minimum: 2 servos (index + middle fingers for scissors)
  - Recommended: 5 servos (all fingers) + optional wrist servo
  - SG90 or MG90S servos work well

### Other Components
- Jumper wires for UART connection
- Breadboard or PCB for servo connections
- **5V external power supply for servos** (ESP32-CAM cannot power multiple servos)

## Wiring Diagram

### UART Connection (ESP32-CAM #1 ↔ ESP32-CAM #2)

```
ESP32-CAM #1                ESP32-CAM #2
(Camera)                    (Servo)
─────────────────           ─────────────────
GPIO4 (TX) ────────────────> GPIO4 (RX)
GPIO33 (RX) <─────────────── GPIO33 (TX)
GND ───────────────────────> GND (IMPORTANT!)
5V ────────────────────────> 5V (optional, for power sharing)
```

**Important**: Connect GND between both ESP32-CAMs for reliable UART communication!

### Servo Connections to ESP32-CAM #2

```
ESP32-CAM #2                 Servo
─────────────────           ─────────────────
GPIO12 ────────────────────> Thumb Servo Signal
GPIO13 ────────────────────> Index Servo Signal
GPIO14 ────────────────────> Middle Servo Signal
GPIO15 ────────────────────> Ring Servo Signal (optional)
GPIO2  ────────────────────> Pinky Servo Signal (optional)
GPIO16 ────────────────────> Wrist Servo Signal (optional)

5V ────────────────────────> All Servos VCC (red wires)
GND ───────────────────────> All Servos GND (brown/black wires)
```

**Note**: Use external 5V power supply for servos. Connect supply GND to ESP32-CAM GND.

### ESP32-CAM Available GPIO Pins

On the AI-Thinker ESP32-CAM, these pins are generally available:
- **GPIO1, GPIO3** - UART0 (USB serial - used for programming)
- **GPIO2** - Available (used for LED on boot, but OK for servos)
- **GPIO4** - Available (used here for UART TX)
- **GPIO12, GPIO13, GPIO14, GPIO15** - Available (used for servos)
- **GPIO16** - Available (used for wrist servo)
- **GPIO32, GPIO33** - Available (GPIO33 used here for UART RX)

**Avoid**: GPIO0, GPIO5 (camera), GPIO26, GPIO27 (camera I2C), etc.

## Software Setup

### 1. Install Arduino IDE 2.x

Download from: https://www.arduino.cc/en/software

### 2. Install ESP32 Board Package

1. Open Arduino IDE
2. Go to **File** → **Preferences**
3. Add this URL to "Additional Boards Manager URLs":
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
4. Go to **Tools** → **Board** → **Boards Manager**
5. Search for "esp32" and install "ESP32 by Espressif Systems"

### 3. Upload Code to ESP32-CAM #2 (Servo)

**ESP32-CAM Upload Procedure:**
1. Open `esp32_servo/esp32_servo.ino`
2. Select board: **AI Thinker ESP32-CAM**
3. **Disconnect GPIO0 from GND** (if connected)
4. Connect GPIO0 to GND (this puts it in upload mode)
5. Click Upload
6. When you see "Connecting.....", **press and hold RESET button**
7. Release RESET when you see "Writing at ..."
8. After upload, **disconnect GPIO0 from GND**
9. Press RESET

Verify in Serial Monitor (115200 baud):
```
========================================
   ESP32-CAM #2 - Servo Controller
========================================
UART1 initialized on RX=GPIO33 TX=GPIO4
Servos initialized...
Waiting for PLAY command from ESP32 #1
```

### 4. Upload Code to ESP32-CAM #1 (Camera)

Same upload procedure as above:
1. Open `esp32_camera/esp32_camera.ino`
2. Select board: **AI Thinker ESP32-CAM**
3. Follow the same GPIO0/RESET procedure
4. Press RESET after upload

Verify in Serial Monitor:
```
========================================
   ESP32-CAM #1 - Camera & Game Logic
========================================
UART1 initialized...
Camera initialized...
AP IP address: 192.168.4.1
HTTP server started
```

### 5. Python Server Setup

1. Install required packages:
   ```bash
   pip install flask opencv-python mediapipe numpy requests
   ```

2. Run the server:
   ```bash
   python web_gesture_server.py
   ```

3. Connect to ESP32-CAM #1 WiFi:
   - SSID: `ESP32-CAM`
   - Password: `12345678`

4. Open browser: `http://localhost:5000`

## UART Protocol

### Command Format

**ESP32-CAM #1 → ESP32-CAM #2:**
```
PLAY\n
```

**ESP32-CAM #2 → ESP32-CAM #1:**
```
MOVE:rock\n
MOVE:paper\n
MOVE:scissors\n
```

## Servo Position Calibration

The default servo positions may not work for your specific hand mechanism. Adjust these values in `esp32_servo.ino`:

```cpp
// ROCK: All fingers closed (fist)
const int rock_pos[] = {
  2400,  // GPIO12 - Thumb - ADJUST THIS
  2300,  // GPIO13 - Index - ADJUST THIS
  2300,  // GPIO14 - Middle - ADJUST THIS
  2300,  // GPIO15 - Ring - ADJUST THIS
  2300,  // GPIO2 - Pinky - ADJUST THIS
  1500   // GPIO16 - Wrist - ADJUST THIS
};
```

### Calibration Tips

1. Use the Serial Monitor to see which gesture is being displayed
2. Test each position individually
3. Values range from 500 (0°) to 2500 (180°) microseconds
4. 1500 is the center (90°) position
5. Fine-tune values for your specific servo and mechanism

## Troubleshooting

### UART Not Working

1. **Check TX/RX cross-connection**: TX of one goes to RX of the other
2. **Check GND connection**: Both ESP32-CAMs must share common ground
3. **Check baud rate**: Both must use 115200
4. **Check GPIO pins**: Verify GPIO4 and GPIO33 are properly connected

### Servos Not Moving

1. **Check power supply**: Use external 5V for servos (ESP32-CAM cannot supply enough current)
2. **Check signal wiring**: Correct GPIO pins (12, 13, 14, 15, 2, 16)
3. **Check servo channels**: Each servo needs unique channel (0-5)
4. **Test with simple code**: Use ESP32 servo library examples

### Camera Not Streaming

1. **Check power**: ESP32-CAM needs stable 5V, 2A minimum
2. **Check WiFi**: Connect to ESP32-CAM #1 AP
3. **Check browser**: Use http://localhost:5000 not direct IP

### Game Not Working

1. Check Serial Monitor output on both ESP32-CAMs
2. Verify UART messages are being sent/received
3. Check Python server logs for errors
4. Test each component individually

## API Endpoints

The ESP32-CAM #1 provides these HTTP endpoints:

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Status page |
| `/stream` | GET | MJPEG video stream |
| `/play?move=rock` | GET/POST | Play a round |
| `/scores` | GET | Get current scores |
| `/reset` | POST | Reset scores |
| `/status` | GET | Get status |

## File Structure

```
Gesture Detection/
├── esp32_camera/
│   └── esp32_camera.ino      # ESP32-CAM #1 firmware (Camera + Game)
├── esp32_servo/
│   └── esp32_servo.ino       # ESP32-CAM #2 firmware (Servo)
├── web_gesture_server.py     # Python server
├── game_engine_uart.py       # Game logic (UART version)
├── templates/
│   └── index.html            # Web interface
└── UART_SETUP.md             # This file
```

## Game Flow

1. User shows gesture to camera
2. Python server detects gesture using MediaPipe
3. User clicks "PLAY ROUND" button
4. Server sends `/play?move=rock` to ESP32-CAM #1
5. ESP32-CAM #1 sends "PLAY" command via UART to ESP32-CAM #2
6. ESP32-CAM #2 randomly selects a move
7. ESP32-CAM #2 moves servos to show gesture physically
8. ESP32-CAM #2 sends "MOVE:rock" back via UART
9. ESP32-CAM #1 determines winner and updates scores
10. Result displayed on web interface
11. Repeat!

## Pin Summary

### ESP32-CAM #1 (Camera)
| Pin | Function |
|-----|----------|
| GPIO4 | UART TX to ESP32-CAM #2 |
| GPIO33 | UART RX from ESP32-CAM #2 |
| GND | Common ground |
| Camera pins | Standard AI-Thinker config |

### ESP32-CAM #2 (Servo)
| Pin | Function |
|-----|----------|
| GPIO4 | UART RX from ESP32-CAM #1 |
| GPIO33 | UART TX to ESP32-CAM #1 |
| GPIO12 | Thumb servo |
| GPIO13 | Index servo |
| GPIO14 | Middle servo |
| GPIO15 | Ring servo |
| GPIO2 | Pinky servo |
| GPIO16 | Wrist servo (optional) |
| GND | Common ground + Servo GND |

**CRITICAL**: ALWAYS CONNECT GND BETWEEN BOTH ESP32-CAMs FOR UART TO WORK!
