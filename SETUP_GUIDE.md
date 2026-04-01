# ESP32-CAM Gesture Detection Setup

## What You Need
- ESP32-CAM with live stream running
- Computer connected to ESP32-CAM WiFi
- Python installed with required packages

## Step 1: Install Python Dependencies

```bash
pip install opencv-python mediapipe flask
```

## Step 2: Ensure Hand Landmarker Model Exists

Make sure `hand_landmarker.task` is in the same folder. If not, run your original `gesture_detector.py` and it will download automatically.

## Step 3: Start the Gesture Detection Server

```bash
python web_gesture_server.py
```

## Step 4: Open in Browser

Go to: **http://localhost:5000**

## How It Works

```
┌─────────────┐         WiFi          ┌─────────────┐       HTTP       ┌──────────────┐
│ ESP32-CAM   │ ◄─────────────────────► │   Your PC   │ ◄───────────────► │   Browser    │
│ (192.168.4.1)│    ESP32-CAM network   │  Flask App  │    localhost:5000 │  + Display   │
│             │                        │             │                    │  Results     │
└─────────────┘                        └─────────────┘                    └──────────────┘
                                              │
                                              ▼
                                     MediaPipe Gesture
                                     Detection
```

## Troubleshooting

**"Failed to connect to ESP32-CAM"**
- Make sure you're connected to ESP32-CAM WiFi
- Check that ESP32-CAM is powered on
- Verify ESP32-CAM code is running (check Serial Monitor)

**Video not loading**
- Refresh the page
- Check browser console for errors (F12)

**Gesture not detecting**
- Ensure good lighting
- Hand should be clearly visible in frame
- Check that MediaPipe installed correctly

## File Structure

```
Gesture Detection/
├── web_gesture_server.py       # Flask server
├── templates/
│   └── index.html              # Webpage
├── hand_landmarker.task        # MediaPipe model
├── gesture_detector.py         # Original detector
└── esp32cam_stream/
    └── esp32cam_stream.ino     # ESP32-CAM code
```
