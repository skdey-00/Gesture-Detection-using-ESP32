# ESP32 #2 Player Setup Guide

## Overview

This ESP32 acts as the computer opponent for Rock Paper Scissors. It connects to the ESP32-CAM's WiFi network and serves random moves via HTTP.

## Hardware Required

- ESP32 board (any model: ESP32-WROOM, ESP32-WROVER, etc.)
- USB cable for programming

## Software Required

- Arduino IDE 2.x
- ESP32 Board Package for Arduino
- Dependencies: WiFi.h, WebServer.h (built-in with ESP32 core)

## Installation Steps

### 1. Install ESP32 Board in Arduino IDE

1. Open Arduino IDE
2. Go to **File** → **Preferences**
3. Add this URL to "Additional Boards Manager URLs":
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
4. Go to **Tools** → **Board** → **Boards Manager**
5. Search for "esp32" and install "ESP32 by Espressif Systems"

### 2. Upload the Code

1. Open `esp32_player/esp32_player.ino`
2. Select your board: **Tools** → **Board** → **ESP32 Arduino** → *Your Board Model*
3. Select correct COM port
4. Click Upload

### 3. Connect to ESP32-CAM Network

1. Power on ESP32-CAM first
2. Connect your computer to "ESP32-CAM" WiFi (password: 12345678)
3. Power on the ESP32 #2 Player
4. It will automatically connect to the ESP32-CAM network

### 4. Verify Connection

Open Serial Monitor (115200 baud) to see:

```
========================================
   ESP32 Rock Paper Scissors Player
========================================

Connecting to ESP32-CAM
Connected! IP address: 192.168.4.2

HTTP server started on port 81
========================================
  ESP32 Player Ready!
========================================
Access at: http://192.168.4.2:81
API: /move - Get random Rock/Paper/Scissors
========================================
```

## WiFi Settings

The code is pre-configured to connect to:
- **SSID**: ESP32-CAM
- **Password**: 12345678

To change, edit these lines in `esp32_player.ino`:

```cpp
const char* ssid = "ESP32-CAM";
const char* password = "12345678";
```

## API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Status page with test button |
| `/move` | GET | Returns random move JSON |
| `/status` | GET | Returns player status JSON |

### Example Responses

**GET /move**
```json
{
  "move": "rock",
  "total_moves": 5,
  "timestamp": 1234567890
}
```

**GET /status**
```json
{
  "status": "ready",
  "ip": "192.168.4.2",
  "last_move": "paper",
  "total_moves": 10,
  "connected_clients": 1
}
```

## Troubleshooting

### "Failed to connect to WiFi"
- Make sure ESP32-CAM is powered on
- Verify you're using the correct SSID/password
- Check that ESP32-CAM's AP is active

### Wrong IP address
- ESP32 should get IP via DHCP: 192.168.4.2
- If different, update `ESP32_PLAYER_URL` in `web_gesture_server.py`

### Can't upload to ESP32
- Hold BOOT button while clicking Upload
- Try a different USB cable (data cable required)
- Select correct board model in Tools

## Network Diagram

```
┌─────────────────┐         ┌─────────────────┐
│   ESP32-CAM #1  │         │   ESP32 #2      │
│   (AP Mode)     │◄────────┤   (Player)      │
│   192.168.4.1   │  WiFi   │   192.168.4.2   │
└────────┬────────┘         └─────────────────┘
         │
         │ WiFi
         ▼
┌─────────────────┐
│   Computer      │
│   (Python       │
│    Server)      │
└─────────────────┘
```
