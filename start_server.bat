@echo off
echo ======================================
echo ESP32-CAM Gesture Detection Server
echo ======================================
echo.
echo Make sure you are connected to ESP32-CAM WiFi first!
echo.
echo Press Ctrl+C to stop the server
echo ======================================
echo.

py web_gesture_server.py

pause
