╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                                ║
║           🔌 COMPLETE WIRING GUIDE FOR DUAL ESP32-CAM SYSTEM 🔌               ║
║                                                                                ║
║                 Rock Paper Scissors - Gesture Detection Game                   ║
║                                                                                ║
╚════════════════════════════════════════════════════════════════════════════════╝


┌──────────────────────────────────────────────────────────────────────────────┐
│                           PARTS YOU NEED                                    │
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│   2x ESP32-CAM (AI-Thinker model)                                            │
│   2x USB-to-Serial Programmer (CP2102 or similar)                           │
│   5x Servo Motors (SG90 or MG90S)                                           │
│   1x External 5V Power Supply (2A or higher)                                │
│   Jumper Wires (male-to-male, male-to-female)                               │
│   Breadboard or PCB                                                          │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                         🔗 PART 1: UART CONNECTION                            ║
║                     (Between the two ESP32-CAMs)                             ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


     THIS IS THE MOST IMPORTANT CONNECTION - GET THIS RIGHT!

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
│          │          │                    │          │          │
│         GND ├───────┼────────────────────┼──────────┤ GND       │
│          │          │   ⚠️ GND MUST BE   │          │          │
│          │          │      CONNECTED!    │          │          │
│          │          │                    │          │          │
└─────────────────────┘                    └─────────────────────┘


    COLOR CODE SUGGESTION:
    ┌────────────────────────────────────────────────────────────┐
    │  WIRE COLOR   │   CONNECT                                │
    ├────────────────────────────────────────────────────────────┤
    │  🔴 RED       │  GPIO4 #1  →  GPIO4 #2                    │
    │  🟡 YELLOW    │  GPIO33 #1 ←  GPIO33 #2                   │
    │  ⚫ BLACK     │  GND #1    →  GND #2                      │
    └────────────────────────────────────────────────────────────┘


    🔴🟡⚫ SIMPLE VIEW:
    ┌────────────────────────┐         ┌────────────────────────┐
    │   ESP32-CAM #1         │         │   ESP32-CAM #2         │
    │                        │         │                        │
    │  [GPIO 4]  🔴──────────┼─────────▶[GPIO 4]               │
    │  [GPIO 33] 🟡◀─────────┼──────────[GPIO 33]              │
    │  [GND]     ⚫──────────┼─────────▶[GND]                  │
    │                        │         │                        │
    └────────────────────────┘         └────────────────────────┘


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                         🔗 PART 2: SERVO CONNECTIONS                          ║
║                    (Only on ESP32-CAM #2 - Servo Unit)                       ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


     ⚠️ IMPORTANT: Use external 5V power for servos! ESP32-CAM cannot power them!


┌──────────────────────────────────────────────────────────────────────────────┐
│                                                                              │
│                         EXTERNAL 5V POWER SUPPLY                             │
│                           (2A minimum)                                      │
│                                                                              │
│   ┌─────────────────────────────────────────────────────────────┐          │
│   │                                                             │          │
│   │    [+] 5V ─────────────────────────────────────────────┐    │          │
│   │                                                           │    │          │
│   │    [-] GND ───────────────────────────────────────────┐  │    │          │
│   │                                                           │  │    │          │
│   └─────────────────────────────────────────────────────────────┘  │    │          │
│                                                                    │    │          │
└────────────────────────────────────────────────────────────────────┼────┼──────────┘
                                                                     │    │


┌──────────────────────────────────────────────────────────────────────────────┐
│                                                                              │
│                            ESP32-CAM #2                                     │
│                           (SERVO UNIT)                                      │
│                                                                              │
│   ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐                                         │
│   │   │ │   │ │   │ │   │ │   │ │   │     (Pin numbers shown)               │
│   │ 12│ │ 13│ │ 14│ │ 15│ │ 2 │ │ 16│                                         │
│   │   │ │   │ │   │ │   │ │   │ │   │                                         │
│   └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘                                         │
│     │     │     │     │     │     │                                         │
│     │     │     │     │     │     │     SERVO SIGNAL WIRES:                 │
│     ▼     ▼     ▼     ▼     ▼     ▼                                         │
│  ┌────┐┌────┐┌────┐┌────┐┌────┐┌────┐                                       │
│  │    ││    ││    ││    ││    ││    │                                       │
│  │ 🗑️ ││ ☝️ ││ 🖕 ││ 💍 ││ 🖐️ ││ 👋 │     Servo functions                   │
│  │Thumb││Index││Mid ││Ring││Pink││Wrist│                                      │
│  │    ││    ││    ││    ││    ││    │                                       │
│  └────┘└────┘└────┘└────┘└────┘└────┘                                       │
│    1    2    3    4    5    6                                             │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                    🔌 EACH SERVO HAS 3 WIRES:                                ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


     ┌─────────────────────────────────────────────────────────────────────┐
     │                                                                      │
     │         SIGNAL (Yellow/Orange/White)                                 │
     │              │                                                        │
     │              ▼                                                        │
     │         ┌─────────┐                                                  │
     │         │  Servo  │                                                  │
     │         │ Motor   │                                                  │
     │         └────┬────┘                                                  │
     │              │                                                        │
     │    ┌─────────┴─────────┐                                             │
     │    │                   │                                             │
     │    ▼                   ▼                                             │
     │  RED              BLACK/BROWN                                       │
     │  (5V)             (GND)                                             │
     │                                                                      │
     └─────────────────────────────────────────────────────────────────────┘


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
│  🔹 SERVO CONNECTIONS (ESP32-CAM #2 only)                                    │
│  ┌────────────────────┬──────────────────────────────────────────────┐       │
│  │ ESP32-CAM #2       │ SERVO                                        │       │
│  │ (Servo Unit)       │                                              │       │
│  ├────────────────────┼──────────────────────────────────────────────┤       │
│  │ GPIO 12           │ → Servo 1 (Thumb) - SIGNAL wire               │       │
│  │ GPIO 13           │ → Servo 2 (Index) - SIGNAL wire               │       │
│  │ GPIO 14           │ → Servo 3 (Middle) - SIGNAL wire              │       │
│  │ GPIO 15           │ → Servo 4 (Ring) - SIGNAL wire                │       │
│  │ GPIO 2            │ → Servo 5 (Pinky) - SIGNAL wire                │       │
│  │ GPIO 16           │ → Servo 6 (Wrist) - SIGNAL wire (optional)     │       │
│  └────────────────────┴──────────────────────────────────────────────┘       │
│                                                                              │
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  🔹 POWER CONNECTIONS                                                         │
│  ┌────────────────────┬──────────────────────────────────────────────┐       │
│  │ SOURCE            │ CONNECT TO                                   │       │
│  ├────────────────────┼──────────────────────────────────────────────┤       │
│  │ Power Supply 5V    │ → All servo RED wires (through distribution) │       │
│  │ Power Supply GND   │ → All servo BLACK/BROWN wires                 │       │
│  │ Power Supply GND   │ → ESP32-CAM #2 GND (common ground!)          │       │
│  │ ESP32-CAM #2 5V    │ → Do NOT power servos from this!              │       │
│  └────────────────────┴──────────────────────────────────────────────┘       │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                     🎯 STEP-BY-STEP ASSEMBLY                                 ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    STEP 1: Power Up Both ESP32-CAMs
    ────────────────────────────────
    ✓ Connect USB power to ESP32-CAM #1 (Camera)
    ✓ Connect USB power to ESP32-CAM #2 (Servo)
    ✓ Connect external 5V power supply (for servos) - leave OFF for now


    STEP 2: Connect UART Cable Between ESP32-CAMs
    ────────────────────────────────────────────
    ✓ Connect GPIO4 of #1 to GPIO4 of #2
    ✓ Connect GPIO33 of #1 to GPIO33 of #2
    ✓ Connect GND of #1 to GND of #2


    STEP 3: Connect Servo Signal Wires
    ──────────────────────────────────
    ✓ Servo 1 (Thumb)    → GPIO12
    ✓ Servo 2 (Index)    → GPIO13
    ✓ Servo 3 (Middle)   → GPIO14
    ✓ Servo 4 (Ring)     → GPIO15
    ✓ Servo 5 (Pinky)    → GPIO2
    ✓ Servo 6 (Wrist)    → GPIO16 (optional)


    STEP 4: Connect Servo Power Wires
    ─────────────────────────────────
    ⚠️ Make sure external 5V power is OFF!
    ✓ Connect all servo RED wires to +5V from external supply
    ✓ Connect all servo BLACK/BROWN wires to GND from external supply
    ✓ Connect external supply GND to ESP32-CAM #2 GND


    STEP 5: Final Check
    ──────────────────
    ✓ All GNDs connected together (ESP32 #1, ESP32 #2, Power Supply)
    ✓ No 5V/GND swapped anywhere
    ✓ Signal wires connected to correct GPIOs
    ✓ External power supply can deliver 2A or more


    STEP 6: Power On and Test
    ────────────────────────
    ✓ Turn on external 5V power supply
    ✓ Upload firmware to both ESP32-CAMs
    ✓ Open Serial Monitor (115200 baud) to verify
    ✓ Test!


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                      ⚠️ CRITICAL WARNINGS                                     ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    ⛔ DO NOT power servos directly from ESP32-CAM!
    → Use external 5V power supply with 2A+ capacity
    → Connecting servos to ESP32-CAM 5V pin will cause crashes!


    ⛔ GND MUST be connected between all devices!
    → ESP32-CAM #1 GND ←→ ESP32-CAM #2 GND ←→ Power Supply GND
    → UART will NOT work without common ground!


    ⛔ Double-check TX/RX connections!
    → GPIO4 connects to GPIO4 (TX to RX)
    → GPIO33 connects to GPIO33 (RX to TX)
    → Cross-connection is handled internally by UART!


    ⛔ Don't short 5V to GND!
    → This will damage your ESP32-CAM or power supply
    → Check connections before powering on!


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                     📱 ESP32-CAM PINOUT REFERENCE                             ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    ESP32-CAM (AI-Thinker) - View from FRONT (connector side)

    ┌────────────────────────────────────────────────────────────────┐
    │                                                                │
    │   ┌────┐ ┌────┐ ┌────┐ ┌────┐ ┌────┐ ┌────┐ ┌────┐ ┌────┐     │
    │   │ V  │ │ G  │ │ G  │ │ G  │ │ G  │ │ G  │ │ G  │ │ 3  │     │
    │   │ 3  │ │ N  │ │ P  │ │ P  │ │ P  │ │ P  │ │ P  │ │ 3  │     │
    │   │ 3  │ │ D  │ │ I  │ │ I  │ │ I  │ │ I  │ │ I  │ │    │     │
    │   │ V  │ │    │ │ O  │ │ O  │ │ O  │ │ O  │ │ O  │ │    │     │
    │   └────┘ └────┘ └────┘ └────┘ └────┘ └────┘ └────┘ └────┘     │
    │                                                                │
    │   These pins are on the bottom connector:                      │
    │                                                                │
    │   ┌─────────┬─────────┬─────────┬─────────┐                    │
    │   │ GPIO1   │ GPIO2   │ GPIO3   │ GPIO4   │ ← Used for UART   │
    │   │ (TX0)   │ (Servo5)│ (RX0)   │ (TX1)   │                    │
    │   ├─────────┼─────────┼─────────┼─────────┤                    │
    │   │ GPIO12  │ GPIO13  │ GPIO14  │ GPIO15  │ ← Used for Servos │
    │   │ (Servo1)│ (Servo2)│ (Servo3)│ (Servo4)│                    │
    │   ├─────────┼─────────┼─────────┼─────────┤                    │
    │   │ GPIO32  │ GPIO33  │ GPIO34  │ GPIO35  │                    │
    │   │ (LED)   │ (RX1)   │         │         │ ← GPIO33 for UART  │
    │   └─────────┴─────────┴─────────┴─────────┘                    │
    │                                                                │
    │   GPIO16 is on the separate connector (optional wrist servo)   │
    │                                                                │
    └────────────────────────────────────────────────────────────────┘


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                      ✅ FINAL CHECKLIST                                       ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    Before powering on, verify:

    ☐ GPIO4 of ESP32-CAM #1 connected to GPIO4 of ESP32-CAM #2
    ☐ GPIO33 of ESP32-CAM #1 connected to GPIO33 of ESP32-CAM #2
    ☐ GND of both ESP32-CAMs connected together
    ☐ External 5V power supply GND connected to ESP32-CAM #2 GND
    ☐ All servo SIGNAL wires connected to correct GPIOs
    ☐ All servo RED wires connected to +5V (external supply)
    ☐ All servo BLACK wires connected to GND
    ☐ No short circuits (5V touching GND anywhere)
    ☐ External power supply can provide 2A or more
    ☐ Firm connections on all wires (no loose wires!)


    When ready:

    ☐ Upload firmware to ESP32-CAM #1 (Camera)
    ☐ Upload firmware to ESP32-CAM #2 (Servo)
    ☐ Connect to ESP32-CAM WiFi: "ESP32-CAM" / "12345678"
    ☐ Run: python web_gesture_server.py
    ☐ Open browser: http://localhost:5000
    ☐ Test!


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                         🆘 TROUBLESHOOTING                                    ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    PROBLEM: Servos not moving at all
    ──────────────────────────────────
    → Check external 5V power is ON
    → Check all servo connections
    → Verify GND is connected between power supply and ESP32-CAM #2
    → Check Serial Monitor for errors


    PROBLEM: ESP32-CAM keeps crashing/restarting
    ────────────────────────────────────────────
    → Servos powered from ESP32-CAM instead of external supply!
    → Move servo power to external 5V supply
    → Power supply must be 2A or higher


    PROBLEM: UART not working (game won't play)
    ────────────────────────────────────────────
    → Check GND is connected between both ESP32-CAMs
    → Verify GPIO4 and GPIO33 connections
    → Check Serial Monitor on both ESP32-CAMs


    PROBLEM: One servo jittering or not working
    ────────────────────────────────────────────
    → Check SIGNAL wire is on correct GPIO
    → Servo might be damaged - try swapping with another
    → Check power supply has enough capacity


╔════════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                      📞 NEED HELP?                                           ║
║                                                                              ║
╚════════════════════════════════════════════════════════════════════════════════╝


    1. Check the Serial Monitor (115200 baud) on both ESP32-CAMs
    2. Verify all connections against this diagram
    3. Make sure GND is connected between ALL devices
    4. Use a multimeter to check for short circuits

                                GOOD LUCK! 🎮

╚════════════════════════════════════════════════════════════════════════════════╝
