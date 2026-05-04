╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                    📋 STEP BY STEP SETUP GUIDE                              ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 1: INSTALL ARDUINO IDE & ESP32 BOARD                                    ║
╚══════════════════════════════════════════════════════════════════════════════╝

1. Download Arduino IDE 2.x from: https://www.arduino.cc/en/software

2. Open Arduino IDE

3. Add ESP32 Board:
   - Go to File → Preferences
   - In "Additional Boards Manager URLs" add:
     https://espressif.github.io/arduino-esp32/package_esp32_index.json
   - Click OK

4. Install ESP32 Board:
   - Go to Tools → Board → Boards Manager
   - Search "esp32"
   - Install "ESP32 by Espressif Systems"


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 2: INSTALL LIBRARIES                                                     ║
╚══════════════════════════════════════════════════════════════════════════════╝

1. Open Arduino IDE

2. Go to: Sketch → Include Library → Manage Libraries

3. Install these libraries:
   ✓ Search "Adafruit PWM" → Install "Adafruit PWM Servo Driver Library"
   ✓ Search "Wire" → Should be built-in (no install needed)


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 3: WIRE EVERYTHING UP                                                    ║
╚══════════════════════════════════════════════════════════════════════════════╝

Open WIRING_PCA9685_SIMPLE.txt and follow these 3 connections:

═════════════════════════════════════════════════════════════════════════════════
    CONNECTION 1: UART (Between ESP32-CAMs)
═════════════════════════════════════════════════════════════════════════════════

    ESP32-CAM #1 (Camera)          ESP32-CAM #2 (Servo)
    ─────────────────────────────────────────────────────────
    GPIO4  ──────────────────────► GPIO4
    GPIO33 ───────────────────────► GPIO33
    GND    ───────────────────────► GND

═════════════════════════════════════════════════════════════════════════════════
    CONNECTION 2: I2C (ESP32-CAM #2 → PCA9685)
═════════════════════════════════════════════════════════════════════════════════

    ESP32-CAM #2                    PCA9685
    ─────────────────────────────────────────────────────────
    GPIO13 ────────────────────────► SDA
    GPIO12 ────────────────────────► SCL
    3.3V   ────────────────────────► VCC
    GND    ────────────────────────► GND

═════════════════════════════════════════════════════════════════════════════════
    CONNECTION 3: SERVOS (PCA9685 → Servos)
═════════════════════════════════════════════════════════════════════════════════

    PCA9685              Servos
    ─────────────────────────────────────────────────────────
    PWM0  ────────────────► Servo 1 (Thumb) - Signal wire 🟡
    PWM1  ────────────────► Servo 2 (Index) - Signal wire 🟡
    PWM2  ────────────────► Servo 3 (Middle) - Signal wire 🟡
    PWM3  ────────────────► Servo 4 (Ring) - Signal wire 🟡
    PWM4  ────────────────► Servo 5 (Pinky) - Signal wire 🟡
    PWM5  ────────────────► Servo 6 (Wrist) - Signal wire 🟡
    V+    ────────────────► All Servo RED wires 🔁
    GND   ────────────────► All Servo BLACK wires ⚫

    External 5V Power Supply:
    +5V ──────────────────► PCA9685 V+
    GND ───────────────────► PCA9685 GND (and ESP32-CAM GND)


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 4: TEST PCA9685 CONNECTION                                               ║
╚══════════════════════════════════════════════════════════════════════════════╝

1. Open: esp32_servo/i2c_scanner.ino

2. Select Board: Tools → Board → ESP32 Arduino → AI Thinker ESP32-CAM

3. Connect ESP32-CAM #2 to computer via USB programmer

4. UPLOAD (follow ESP32-CAM upload procedure):
   a) Connect GPIO0 to GND
   b) Click Upload
   c) When "Connecting..." appears, press RESET button
   d) Release RESET when "Writing at..." appears
   e) Disconnect GPIO0 from GND
   f) Press RESET

5. Open Serial Monitor (115200 baud)

6. You should see:
   ✓ Found I2C device at 0x40
     → This is the PCA9685! (default address)

7. If NOT found, check wiring!


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 5: UPLOAD FIRMWARE TO ESP32-CAM #2 (SERVO)                              ║
╚══════════════════════════════════════════════════════════════════════════════╝

1. Open: esp32_servo/esp32_servo.ino

2. Verify board: Tools → Board → AI Thinker ESP32-CAM

3. UPLOAD (same procedure as Step 4)

4. Open Serial Monitor (115200 baud)

5. You should see:
   ========================================
      ESP32-CAM #2 - Servo Controller
      with PCA9685 Driver
   ========================================
   PCA9685 initialized!
   Moving servos to neutral position...
   ESP32-CAM Servo Controller Ready!
   Waiting for PLAY command from ESP32 #1

6. Servos should move to center position!


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 6: UPLOAD FIRMWARE TO ESP32-CAM #1 (CAMERA)                             ║
╚══════════════════════════════════════════════════════════════════════════════╝

1. Disconnect ESP32-CAM #2, connect ESP32-CAM #1 to computer

2. Open: esp32_camera/esp32_camera.ino

3. Verify board: Tools → Board → AI Thinker ESP32-CAM

4. UPLOAD (same procedure)

5. Open Serial Monitor (115200 baud)

6. You should see:
   ========================================
      ESP32-CAM #1 - Camera & Game Logic
   ========================================
   UART1 initialized on RX=GPIO33 TX=GPIO4
   Camera initialized!
   AP IP address: 192.168.4.1
   HTTP server started


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 7: INSTALL PYTHON DEPENDENCIES                                           ║
╚══════════════════════════════════════════════════════════════════════════════╝

On your computer, open Command Prompt/Terminal:

1. Navigate to project folder:
   cd "C:\Users\sanme\desktop\Tp projects\Gesture Detection"

2. Install required packages:
   pip install flask opencv-python mediapipe numpy requests


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 8: POWER UP BOTH ESP32-CAMs                                             ║
╚══════════════════════════════════════════════════════════════════════════════╝

1. Connect both ESP32-CAMs to USB power (or computer)

2. Turn on external 5V power supply for servos

3. Wait 10 seconds for them to boot up


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 9: CONNECT TO ESP32-CAM WIFI                                             ║
╚══════════════════════════════════════════════════════════════════════════════╝

On your computer:

1. Open WiFi settings

2. Connect to:
   SSID: ESP32-CAM
   Password: 12345678


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 10: RUN PYTHON SERVER                                                    ║
╚══════════════════════════════════════════════════════════════════════════════╝

1. Open Command Prompt/Terminal

2. Navigate to project folder:
   cd "C:\Users\sanme\desktop\Tp projects\Gesture Detection"

3. Run server:
   python web_gesture_server.py

4. You should see:
   Testing connection to http://192.168.4.1/stream...
   Connection successful! Frame size: (240, 320)
   Initializing game engine...
   Game engine ready
   Gesture Detection Server Started!
   Open in browser: http://localhost:5000


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 11: OPEN WEB BROWSER                                                      ║
╚══════════════════════════════════════════════════════════════════════════════╝

1. Open your browser

2. Go to: http://localhost:5000

3. You should see:
   ✓ Live video feed from camera
   ✓ "Detected Gesture" box
   ✓ "Rock Paper Scissors" game section
   ✓ Scoreboard


╔══════════════════════════════════════════════════════════════════════════════╗
║ STEP 12: PLAY THE GAME!                                                        ║
╚══════════════════════════════════════════════════════════════════════════════╝

1. Show your hand to the camera:
   ✊ Closed fist = ROCK
   ✋ Open hand = PAPER
   ✌️ Index + Middle = SCISSORS

2. Click "PLAY ROUND" button

3. Watch:
   - ESP32 #2 will randomly select a move
   - Servos will move to show the gesture
   - Winner is decided
   - Score updates!

4. Keep playing! Scores are tracked automatically.


╔══════════════════════════════════════════════════════════════════════════════╗
║                     🆘 TROUBLESHOOTING                                       ║
╚══════════════════════════════════════════════════════════════════════════════╝

PROBLEM: PCA9685 not detected
SOLUTION: Check I2C wiring, run i2c_scanner.ino

PROBLEM: Servos not moving
SOLUTION: Check external 5V power, check PWM connections

PROBLEM: Camera not showing
SOLUTION: Connect to ESP32-CAM WiFi, check power supply

PROBLEM: Game not working
SOLUTION: Check UART wires, check Serial Monitor output

PROBLEM: ESP32-CAM won't upload
SOLUTION: Hold RESET during upload, use GPIO0 to GND trick


╔══════════════════════════════════════════════════════════════════════════════╗
║                    ✅ YOU'RE DONE! HAVE FUN! 🎮                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
