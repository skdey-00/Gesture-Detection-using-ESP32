# Rock Paper Scissors Gesture Detection

A real-time hand gesture detection system that recognizes **Rock**, **Paper**, and **Scissors** gestures using MediaPipe and OpenCV. Supports both webcam and ESP32-CAM streaming.

![Gesture Detection](https://img.shields.io/badge/Hand-Gesture-blue)
![MediaPipe](https://img.shields.io/badge/MediaPipe-0.10+-green)
![Python](https://img.shields.io/badge/Python-3.8+-yellow)

## Features

- **Real-time gesture detection** using MediaPipe hand landmark detection
- **Webcam support** - detect gestures from your computer's webcam
- **ESP32-CAM support** - wireless camera streaming for portable projects
- **Web interface** - view live detection results in your browser
- **Multi-platform** - works on Windows, macOS, and Linux

## How It Works

### Hand Detection
The system uses **MediaPipe's Hand Landmarker** which detects 21 keypoints (landmarks) on each hand:

```
     Wrist -> 0
            /    \
        Thumb     Index -> 8
          |        |
          4        |
                 Middle -> 12
                   |
                 Ring -> 16
                   |
                 Pinky -> 20
```

### Gesture Classification
Gestures are classified based on **finger extension patterns**:

| Gesture | Description | Finger Pattern |
|---------|-------------|----------------|
| **Rock** ✊ | Closed fist | 0 fingers extended |
| **Paper** ✋ | Open hand | All 5 fingers extended |
| **Scissors** ✌️ | V-sign | Index + Middle fingers extended |

### Finger Detection Algorithm
For each finger, the system compares:
- **Tip position** vs **PIP (middle joint)** position
- **Distance from wrist** to tip vs PIP joint

A finger is considered "extended" when:
- The tip is above the PIP joint (lower Y value in image coordinates)
- The tip is farther from the wrist than the PIP joint

## Hardware Requirements

### For Webcam Mode
- Computer with built-in or external webcam
- USB 2.0+ port

### For ESP32-CAM Mode
- **ESP32-CAM** module (AI-Thinker ESP32-CAM recommended)
- **Micro USB cable** for programming
- **FTDI/USB-to-TTL adapter** (for Serial Monitor, optional)
- **5V power supply** (or use USB power)
- **Jumper wires** for connections

## Software Requirements

- **Python 3.8+**
- **pip** (Python package manager)

## Installation

### 1. Clone or Download this Repository

```bash
git clone https://github.com/skdey-00/Gesture-Detection-using-ESP32.git
cd Gesture-Detection-using-ESP32
```

### 2. Install Python Dependencies

```bash
pip install -r requirements.txt
```

**Dependencies:**
- `opencv-python` - Computer vision and image processing
- `mediapipe` - Hand landmark detection
- `numpy` - Numerical operations
- `flask` - Web server (for ESP32-CAM mode)

### 3. Download the AI Model (Automatic)

On first run, the `hand_landmarker.task` model file will be automatically downloaded from Google's servers.

## Usage

### Mode 1: Webcam Detection

Run the standalone detector:

```bash
py gesture_detector.py
```

Or:

```bash
python gesture_detector.py
```

**Options:**
- **Option 1** - Real-time webcam detection
- **Option 2** - Detect from a single image file

**Controls:**
- Press **'q'** to quit the webcam view

### Mode 2: ESP32-CAM Wireless Detection

#### Step 1: Setup ESP32-CAM Hardware

Connect your ESP32-CAM:

```
ESP32-CAM          FTDI/USB-Serial
--------          ----------------
VCC      ------>   5V
GND      ------>   GND
RX       ------>   TX
TX       ------>   RX
```

#### Step 2: Upload the Streaming Sketch

1. Open Arduino IDE
2. Install **ESP32 board support** (if not already installed)
3. Open `esp32cam_stream/esp32cam_stream.ino`
4. Configure your WiFi credentials in the sketch:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

5. Select your board: **Tools → Board → ESP32 Arduino → AI Thinker ESP32-CAM**
6. Connect GPIO0 to GND (to enter upload mode)
7. Upload the sketch
8. Disconnect GPIO0 from GND and press ESP32 reset button

#### Step 3: Connect to ESP32-CAM

1. Connect your computer to ESP32-CAM's WiFi hotspot
2. Default IP: `http://192.168.4.1/stream`

#### Step 4: Start the Web Server

**Windows:**
```bash
start_server.bat
```

**Or manually:**
```bash
py web_gesture_server.py
```

#### Step 5: View in Browser

Open: **http://localhost:5000**

## Project Structure

```
Gesture-Detection-using-ESP32/
├── gesture_detector.py       # Standalone gesture detector
├── web_gesture_server.py     # Flask web server
├── hand_landmarker.task      # MediaPipe AI model
├── requirements.txt          # Python dependencies
├── start_server.bat          # Windows startup script
│
├── esp32cam_stream/          # ESP32-CAM Arduino sketch
│   ├── esp32cam_stream.ino
│   └── camera_pins.h
│
├── gestures/                 # Training/dataset images
│   ├── Rock/
│   ├── Paper/
│   └── scissors/
│
└── templates/                # Web interface
    └── index.html
```

## API Endpoints (Web Server)

| Endpoint | Description |
|----------|-------------|
| `/` | Main web interface |
| `/processed_feed` | Live MJPEG video stream with detection overlay |
| `/gesture` | JSON response with current gesture |
| `/status` | Server status and connection info |

## Troubleshooting

### "No module named 'mediapipe'"
```bash
pip install mediapipe
```

### "Could not open webcam"
- Check if another application is using the webcam
- Try a different USB port
- Verify webcam permissions in your OS settings

### "Could not connect to ESP32-CAM"
- Ensure ESP32-CAM is powered on
- Verify you're connected to the ESP32-CAM WiFi (not your regular WiFi)
- Check the IP address in `web_gesture_server.py` matches your ESP32-CAM's IP
- Try resetting the ESP32-CAM

### Poor detection accuracy
- Ensure good lighting (avoid backlighting)
- Keep your hand clearly visible in frame
- Make distinct gestures (fully extend/close fingers)

### High latency in video stream
- Reduce ESP32-CAM resolution in the Arduino sketch
- Check WiFi signal strength
- Close other bandwidth-intensive applications

## How Gesture Detection Works (Deep Dive)

```
┌─────────────────┐
│  Input Frame    │
│  (RGB Image)    │
└────────┬────────┘
         │
         ▼
┌─────────────────────────────────┐
│  MediaPipe Hand Landmarker      │
│  - Detects hand(s) in frame      │
│  - Returns 21 landmarks per hand │
└────────┬────────────────────────┘
         │
         ▼
┌─────────────────────────────────┐
│  Finger Extension Analysis      │
│  For each finger:               │
│  1. Get tip and PIP coordinates │
│  2. Compare Y positions          │
│  3. Compare wrist distances     │
│  4. Determine: extended (1)     │
│     or closed (0)               │
└────────┬────────────────────────┘
         │
         ▼
┌─────────────────────────────────┐
│  Gesture Classification         │
│  Based on finger pattern:       │
│  - [0,0,0,0,0] = Rock          │
│  - [1,1,1,1,1] = Paper         │
│  - [?,1,1,0,0] = Scissors      │
└────────┬────────────────────────┘
         │
         ▼
┌─────────────────────────────────┐
│  Output: Gesture Label          │
│  + Visual overlay on frame      │
└─────────────────────────────────┘
```

## Landmark Indices

```
Hand Landmark Reference:
 0: Wrist
 4: Thumb Tip
 8: Index Finger Tip
12: Middle Finger Tip
16: Ring Finger Tip
20: Pinky Tip

Joints (PIP):
 6: Index PIP
10: Middle PIP
14: Ring PIP
18: Pinky PIP
```

## Contributing

Feel free to submit issues and enhancement requests!

## License

This project is open source and available under the MIT License.

## Acknowledgments

- [MediaPipe](https://google.github.io/mediapipe/) by Google for hand tracking
- [OpenCV](https://opencv.org/) for computer vision operations
- [Flask](https://flask.palletsprojects.com/) for the web server

---

**Made with ❤️ by skdey-00**
