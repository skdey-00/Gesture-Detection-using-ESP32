@echo off
echo ======================================
echo ESP32-CAM Gesture Detection Server
echo Rock Paper Scissors Game
echo ======================================
echo.
echo Make sure both ESP32 devices are ready:
echo 1. ESP32-CAM (WiFi AP at 192.168.4.1)
echo 2. ESP32 Player (Connected to AP at 192.168.4.2)
echo.
echo Press Ctrl+C to stop the server
echo ======================================
echo.

py web_gesture_server.py

pause
