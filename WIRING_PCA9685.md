╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║           🔌 COMPLETE WIRING GUIDE - PCA9685 SERVO DRIVER 🔌                ║
║                                                                              ║
║                 Rock Paper Scissors - Gesture Detection Game                  ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


┌──────────────────────────────────────────────────────────────────────────────┐
│                           PARTS YOU NEED                                    │
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│   2x ESP32-CAM (AI-Thinker model)                                            │
│   1x PCA9685 16-Channel PWM/Servo Driver                                     │
│   2x USB-to-Serial Programmer                                                │
│   5x Servo Motors (SG90 or MG90S) - up to 16 supported!                     │
│   1x External 5V Power Supply (2A or higher, depends on servo count)        │
│   Jumper Wires                                                               │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                    🔗 PART 1: UART CONNECTION                                 ║
║                 (Between the two ESP32-CAMs)                                  ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    THIS CONNECTION STAYS THE SAME!

┌─────────────────────┐                    ┌─────────────────────┐
│                     │                    │                     │
│    ESP32-CAM #1     │                    │    ESP32-CAM #2     │
│   (CAMERA UNIT)     │                    │   (SERVO UNIT)      │
│                     │                    │                     │
│  ┌───────────────┐  │                    │  ┌───────────────┐  │
│  │               │  │                    │  │               │  │
│  │   GPIO 4   TX ├──┼────────────────────┼─┼> RX  GPIO 4   │  │
│  │      ▼        │  │                    │  │       ▲        │  │
│  │               │  │    UART CABLE       │  │               │  │
│  │               │  │                    │  │               │  │
│  │   GPIO 33  RX ├──┼────────────────────┼─┼── TX GPIO 33  │  │
│  │      ▲        │  │                    │  │       ▼        │  │
│  │               │  │                    │  │               │  │
│  └───────┬───────┘  │                    │  └───────┬───────┘  │
│          │          │                    │          │          │
│         GND ├───────┼────────────────────┼──────────┤ GND       │
│          │          │   ⚠️ GND MUST BE   │          │          │
│          │          │      CONNECTED!    │          │          │
└─────────────────────┘                    └─────────────────────┘


    COLOR CODE SUGGESTION:
    ┌────────────────────────────────────────────────────────────┐
    │  WIRE COLOR   │   CONNECT                                │
    ├────────────────────────────────────────────────────────────┤
    │  🔴 RED       │  GPIO4 #1  →  GPIO4 #2                    │
    │  🟡 YELLOW    │  GPIO33 #1 ←  GPIO33 #2                   │
    │  ⚫ BLACK     │  GND #1    →  GND #2                      │
    └────────────────────────────────────────────────────────────┘


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                    🔗 PART 2: I2C TO PCA9685                                 ║
║                    (ESP32-CAM #2 → PCA9685 Board)                            ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    ONLY 2 WIRES NEEDED! (Plus power)

┌─────────────────────┐                    ┌─────────────────────┐
│    ESP32-CAM #2     │                    │     PCA9685        │
│   (SERVO UNIT)      │                    │   Servo Driver     │
│                     │   I2C CABLE        │                     │
│  ┌───────────────┐  │                    │  ┌───────────────┐  │
│  │               │  │                    │  │               │  │
│  │   GPIO 13 SDA ├──┼────────────────────┼─┼─ SDA          │  │
│  │      ▼        │  │                    │  │       ▲       │  │
│  │               │  │                    │  │               │  │
│  │   GPIO 12 SCL ├──┼────────────────────┼─┼─ SCL          │  │
│  │      ▼        │  │                    │  │       ▲       │  │
│  │               │  │                    │  │               │  │
│  │   3.3V        ├──┼────────────────────┼─┼─ VCC          │  │
│  │               │  │                    │  │               │  │
│  │   GND         ├──┼────────────────────┼─┼─ GND          │  │
│  └───────────────┘  │                    │  └───────────────┘  │
└─────────────────────┘                    └─────────────────────┘


    🟦 BLUE    = SDA (GPIO13 → SDA)
    🟪 PURPLE  = SCL (GPIO12 → SCL)
    🔴 RED     = 3.3V (ESP32) → VCC (PCA9685) - powers the chip
    ⚫ BLACK   = GND → GND


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                    🔗 PART 3: SERVOS TO PCA9685                              ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


            PCA9685 SERVO DRIVER BOARD
    ┌─────────────────────────────────────────────┐
    │                                             │
    │  ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐      │
    │  │ 0 │ │ 1 │ │ 2 │ │ 3 │ │ 4 │ │ 5 │ ... │  │  ← PWM Outputs
    │  └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘      │
    │    │     │     │     │     │     │         │
    │    ▼     ▼     ▼     ▼     ▼     ▼         │
    │  ┌─────┐┌─────┐┌─────┐┌─────┐┌─────┐┌─────┐│
    │  │Thumb││Index││Mid  ││Ring ││Pink ││Wrist││
    │  └─────┘└─────┘└─────┘└─────┘└─────┘└─────┘│
    │                                             │
    │  [+] ──────────────────┐                   │  ← External 5V
    │  [-] ──────────────┐   │                   │     (servo power)
    │                     │   │                   │
    └─────────────────────┼───┼───────────────────┘
                          │   │


    EACH SERVO CONNECTS TO PCA9685:

    ┌────────────────────────────────────────────────────────────────────┐
    │                                                                    │
    │   PCA9685 Channel 0          PCA9685 Channel 1                    │
    │   ┌─────────┐                ┌─────────┐                         │
    │   │  PWM    │                │  PWM    │                         │
    │   │  OUT 0  │                │  OUT 1  │                         │
    │   └────┬────┘                └────┬────┘                         │
    │        │  🟡 Signal               │  🟡 Signal                  │
    │        ▼                         ▼                               │
    │     ┌─────┐                   ┌─────┐                            │
    │     │     │                   │     │                            │
    │     │ 🗑️  │  Thumb            │ ☝️  │  Index                     │
    │     │     │                   │     │                            │
    │     └──┬──┘                   └──┬──┘                            │
    │        │  🔴 +5V                 │  🔴 +5V                        │
    │        └──────────┬───────────────┘                               │
    │                   ▼                                               │
    │            External 5V Supply                                       │
    │                   │                                               │
    │        ⚫ GND    ⚫ GND                                            │
    │            └───────┴───────→ GND (to ESP32 + Supply)              │
    │                                                                    │
    └────────────────────────────────────────────────────────────────────┘


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                     📋 COMPLETE WIRING TABLE                                 ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


┌──────────────────────────────────────────────────────────────────────────────┐
│                                                                              │
│  🔹 UART CONNECTION (ESP32-CAM #1 ↔ ESP32-CAM #2)                            │
│  ┌────────────────────┬──────────────────────────────────────────────┐       │
│  │ ESP32-CAM #1       │ CONNECT TO                                   │       │
│  │ (Camera Unit)      │                                              │       │
│  ├────────────────────┼──────────────────────────────────────────────┤       │
│  │ GPIO 4            │ → ESP32-CAM #2 GPIO 4                         │       │
│  │ GPIO 33           │ → ESP32-CAM #2 GPIO 33                        │       │
│  │ GND               │ → ESP32-CAM #2 GND + Power Supply GND         │       │
│  └────────────────────┴──────────────────────────────────────────────┘       │
│                                                                              │
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  🔹 I2C CONNECTION (ESP32-CAM #2 → PCA9685)                                  │
│  ┌────────────────────┬──────────────────────────────────────────────┐       │
│  │ ESP32-CAM #2       │ CONNECT TO                                   │       │
│  │ (Servo Unit)       │                                              │       │
│  ├────────────────────┼──────────────────────────────────────────────┤       │
│  │ GPIO 13 (SDA)     │ → PCA9685 SDA pin                             │       │
│  │ GPIO 12 (SCL)     │ → PCA9685 SCL pin                             │       │
│  │ 3.3V              │ → PCA9685 VCC pin                             │       │
│  │ GND               │ → PCA9685 GND pin                             │       │
│  └────────────────────┴──────────────────────────────────────────────┘       │
│                                                                              │
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  🔹 SERVO CONNECTIONS (PCA9685 → Servos)                                    │
│  ┌────────────────────┬──────────────────────────────────────────────┐       │
│  │ PCA9685            │ SERVO                                        │       │
│  ├────────────────────┼──────────────────────────────────────────────┤       │
│  │ Channel 0 (PWM0)   │ → Servo 1 (Thumb) - SIGNAL (🟡)               │       │
│  │ Channel 1 (PWM1)   │ → Servo 2 (Index) - SIGNAL (🟡)               │       │
│  │ Channel 2 (PWM2)   │ → Servo 3 (Middle) - SIGNAL (🟡)              │       │
│  │ Channel 3 (PWM3)   │ → Servo 4 (Ring) - SIGNAL (🟡)                │       │
│  │ Channel 4 (PWM4)   │ → Servo 5 (Pinky) - SIGNAL (🟡)               │       │
│  │ Channel 5 (PWM5)   │ → Servo 6 (Wrist) - SIGNAL (🟡) (optional)    │       │
│  │ VCC (+) terminal   │ → All servo RED wires (🔴) - from ext 5V       │       │
│  │ GND (-) terminal   │ → All servo BLACK/BROWN wires (⚫)             │       │
│  └────────────────────┴──────────────────────────────────────────────┘       │
│                                                                              │
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  🔹 POWER CONNECTIONS                                                         │
│  ┌────────────────────┬──────────────────────────────────────────────┐       │
│  │ SOURCE            │ CONNECT TO                                   │       │
│  ├────────────────────┼──────────────────────────────────────────────┤       │
│  │ Power Supply 5V    │ → PCA9685 V+ terminal                        │       │
│  │ Power Supply GND   │ → PCA9685 GND terminal                       │       │
│  │ Power Supply GND   │ → ESP32-CAM #2 GND (common ground!)          │       │
│  │ ESP32-CAM #2 3.3V  │ → PCA9685 VCC (logic power)                  │       │
│  │ ESP32-CAM #2 GND   │ → PCA9685 GND (common ground)                 │       │
│  └────────────────────┴──────────────────────────────────────────────┘       │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                    🎯 STEP-BY-STEP ASSEMBLY                                 ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    STEP 1: Power Up Both ESP32-CAMs
    ────────────────────────────────
    ✓ Connect USB power to ESP32-CAM #1 (Camera)
    ✓ Connect USB power to ESP32-CAM #2 (Servo)
    ✓ Connect external 5V power supply - leave OFF for now


    STEP 2: Connect UART Cable Between ESP32-CAMs
    ────────────────────────────────────────────
    ✓ Connect GPIO4 of #1 to GPIO4 of #2
    ✓ Connect GPIO33 of #1 to GPIO33 of #2
    ✓ Connect GND of #1 to GND of #2


    STEP 3: Connect I2C Cable (ESP32-CAM #2 → PCA9685)
    ────────────────────────────────────────────────────
    ✓ Connect GPIO13 (SDA) to PCA9685 SDA
    ✓ Connect GPIO12 (SCL) to PCA9685 SCL
    ✓ Connect 3.3V to PCA9685 VCC
    ✓ Connect GND to PCA9685 GND


    STEP 4: Connect Servos to PCA9685
    ──────────────────────────────────
    ✓ Servo 1 (Thumb)    → Channel 0
    ✓ Servo 2 (Index)    → Channel 1
    ✓ Servo 3 (Middle)   → Channel 2
    ✓ Servo 4 (Ring)     → Channel 3
    ✓ Servo 5 (Pinky)    → Channel 4
    ✓ Servo 6 (Wrist)    → Channel 5 (optional)
    ✓ All servo RED wires → V+ terminal
    ✓ All servo BLACK wires → GND terminal


    STEP 5: Connect Power to PCA9685
    ──────────────────────────────
    ⚠️ Make sure external 5V power is OFF!
    ✓ Connect +5V to PCA9685 V+ terminal
    ✓ Connect GND to PCA9685 GND terminal
    ✓ Connect Power Supply GND to ESP32-CAM #2 GND


    STEP 6: Final Check
    ──────────────────
    ✓ All GNDs connected together (ESP32 #1, ESP32 #2, PCA9685, Power Supply)
    ✓ No 5V/GND swapped anywhere
    ✓ I2C wires connected correctly (SDA→SDA, SCL→SCL)
    ✓ Servos connected to correct channels
    ✓ External power supply can deliver adequate current


    STEP 7: Power On and Test
    ────────────────────────
    ✓ Turn on external 5V power supply
    ✓ Upload firmware to both ESP32-CAMs
    ✓ Open Serial Monitor (115200 baud) to verify
    ✓ Check for "PCA9685 initialized!" message
    ✓ Test!


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                      ⚠️ CRITICAL WARNINGS                                     ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    ⛔ Use external 5V for servo power!
    → PCA9685 V+ connects to external 5V supply
    → ESP32-CAM 3.3V powers the PCA9685 logic (VCC pin)
    → Do NOT power servos from ESP32-CAM!


    ⛔ GND MUST be connected between all devices!
    → ESP32-CAM #1 GND ←→ ESP32-CAM #2 GND ←→ PCA9685 GND ←→ Power Supply GND
    → I2C and UART will NOT work without common ground!


    ⛔ Double-check I2C connections!
    → SDA connects to SDA (not crossed)
    → SCL connects to SCL (not crossed)
    → I2C is NOT like UART (TX/RX are not crossed!)


    ⛔ Check PCA9685 address!
    → Default is 0x40 (A5, A4, A3, A2, A1, A0 pins = LOW)
    → If you changed jumpers, update the address in code!


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                    📋 PCA9685 CHANNEL MAPPING                                 ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    ┌─────────┬─────────┬─────────────────────────────────┐
    │ Channel │ Finger  │ Servo Wire                     │
    ├─────────┼─────────┼─────────────────────────────────┤
    │    0    │ Thumb   │ 🟡 Signal → PWM0               │
    │    1    │ Index   │ 🟡 Signal → PWM1               │
    │    2    │ Middle  │ 🟡 Signal → PWM2               │
    │    3    │ Ring    │ 🟡 Signal → PWM3               │
    │    4    │ Pinky   │ 🟡 Signal → PWM4               │
    │    5    │ Wrist   │ 🟡 Signal → PWM5 (optional)    │
    └─────────┴─────────┴─────────────────────────────────┘

    All servos:
    🔴 RED wire → V+ terminal (from external 5V)
    ⚫ BLACK wire → GND terminal


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                      ✅ FINAL CHECKLIST                                       ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    Before powering on, verify:

    ☐ UART: GPIO4 #1 ↔ GPIO4 #2
    ☐ UART: GPIO33 #1 ↔ GPIO33 #2
    ☐ I2C: GPIO13 (SDA) → PCA9685 SDA
    ☐ I2C: GPIO12 (SCL) → PCA9685 SCL
    ☐ Power: 3.3V → PCA9685 VCC
    ☐ Power: External 5V → PCA9685 V+
    ☐ GND: All devices connected together
    ☐ Servos: Connected to correct PWM channels
    ☐ Servos: All RED wires to V+
    ☐ Servos: All BLACK wires to GND
    ☐ No short circuits


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                         🆘 TROUBLESHOOTING                                    ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    PROBLEM: PCA9685 not detected
    ────────────────────────────
    → Check I2C wiring (SDA→SDA, SCL→SCL)
    → Verify GND is connected
    → Check if address jumpers on PCA9685 match code (default 0x40)
    → Try scanning I2C bus with scanner sketch


    PROBLEM: Servos jittering or not moving
    ─────────────────────────────────────
    → Check external 5V power is connected to V+ terminal
    → Verify power supply can provide enough current (2A+ for 5 servos)
    → Check servo signal wires are in correct PWM channels
    → Servo might be damaged - try swapping


    PROBLEM: One specific servo not working
    ─────────────────────────────────────
    → Swap servo with another to test if servo is bad
    → Check PWM channel connection
    → Try a different channel


    PROBLEM: ESP32-CAM crashes when servos move
    ────────────────────────────────────────
    → External 5V not connected properly!
    → Servos are drawing power through ESP32-CAM
    → Check V+ terminal has external 5V


    PROBLEM: UART not working
    ──────────────────────────
    → Check GND connected between both ESP32-CAMs
    → Verify GPIO4 and GPIO33 connections
    → Check Serial Monitor output


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║              🔌 WIRE COLOR GUIDE FOR YOUR TEAM                                ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    ┌─────────────┬────────────────────────────────────────────┐
    │ WIRE COLOR  │ CONNECTION                                 │
    ├─────────────┼────────────────────────────────────────────┤
    │ 🔴 RED      │ UART: GPIO4 #1 → GPIO4 #2                  │
    │ 🟡 YELLOW   │ UART: GPIO33 #1 → GPIO33 #2                 │
    │ ⚫ BLACK    │ UART: GND #1 → GND #2                       │
    ├─────────────┼────────────────────────────────────────────┤
    │ 🟦 BLUE     │ I2C: GPIO13 (SDA) → PCA9685 SDA             │
    │ 🟪 PURPLE   │ I2C: GPIO12 (SCL) → PCA9685 SCL             │
    │ 🔴 RED      │ I2C: 3.3V → PCA9685 VCC                     │
    │ ⚫ BLACK    │ I2C: GND → PCA9685 GND                      │
    ├─────────────┼────────────────────────────────────────────┤
    │ 🟡 YELLOW   │ Servo signal wires (any color works)        │
    │ 🔴 RED      │ All servo RED wires → PCA9685 V+            │
    │ ⚫ BLACK    │ All servo BLACK wires → PCA9685 GND         │
    │ 🔴 RED      │ Power supply +5V → PCA9685 V+               │
    │ ⚫ BLACK    │ Power supply GND → PCA9685 GND              │
    └─────────────┴────────────────────────────────────────────┘


                                GOOD LUCK! 🎮

╚════════════════════════════════════════════════════════════════════════════════╝
