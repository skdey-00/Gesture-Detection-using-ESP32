"""
Flask server that connects to ESP32-CAM stream and performs gesture detection.
Displays live video + detection results on a webpage.
"""

import cv2
import numpy as np
import mediapipe as mp
from flask import Flask, render_template, Response, jsonify, stream_with_context
import threading
import time
import sys

app = Flask(__name__)

# ESP32-CAM stream URL
ESP32_STREAM_URL = "http://192.168.4.1/stream"

# Global variables
current_gesture = "No hand"
gesture_lock = threading.Lock()
detector = None
frame_lock = threading.Lock()
latest_frame = None
frame_count = 0


class GestureDetector:
    """Improved gesture detector using MediaPipe."""

    def __init__(self):
        try:
            from mediapipe.tasks.python.vision import HandLandmarker, HandLandmarkerOptions
            from mediapipe.tasks.python import BaseOptions

            base_options = BaseOptions(model_asset_path='hand_landmarker.task')
            options = HandLandmarkerOptions(
                base_options=base_options,
                num_hands=1,
                min_hand_detection_confidence=0.5,
                min_hand_presence_confidence=0.5,
                min_tracking_confidence=0.5
            )
            self.detector = HandLandmarker.create_from_options(options)
            self.available = True
            print("MediaPipe detector initialized successfully")
        except Exception as e:
            print(f"MediaPipe not available: {e}")
            self.available = False

    def count_fingers(self, landmarks):
        """
        Count extended fingers based on landmarks.
        Improved logic for better detection.
        """
        fingers = []

        # Get image dimensions from landmarks
        # landmarks are normalized (0-1)

        # Thumb - special handling
        # Compare thumb tip (4) to thumb IP (3) and thumb MCP (2)
        # Check if thumb is extended outward
        thumb_tip = landmarks[4]
        thumb_ip = landmarks[3]
        thumb_mcp = landmarks[2]
        wrist = landmarks[0]

        # Calculate distance from thumb tip to wrist vs thumb IP to wrist
        thumb_tip_dist = np.sqrt((thumb_tip.x - wrist.x)**2 + (thumb_tip.y - wrist.y)**2)
        thumb_ip_dist = np.sqrt((thumb_ip.x - wrist.x)**2 + (thumb_ip.y - wrist.y)**2)

        # Thumb is extended if tip is farther from wrist than IP joint
        if thumb_tip_dist > thumb_ip_dist * 1.2:
            fingers.append(1)
        else:
            fingers.append(0)

        # Other four fingers - check if tip is above PIP joint
        # In image coordinates, smaller y = higher position
        finger_tips = [8, 12, 16, 20]      # Index, Middle, Ring, Pinky tips
        finger_pips = [6, 10, 14, 18]      # Index, Middle, Ring, Pinky PIP joints

        for tip_idx, pip_idx in zip(finger_tips, finger_pips):
            tip = landmarks[tip_idx]
            pip = landmarks[pip_idx]

            # Also check MCP joint for more accuracy
            mcp_idx = tip_idx - 2
            mcp = landmarks[mcp_idx]

            # Finger is extended if tip is above (smaller y) than PIP
            # And tip is farther from wrist than PIP
            wrist_to_tip = np.sqrt((tip.x - wrist.x)**2 + (tip.y - wrist.y)**2)
            wrist_to_pip = np.sqrt((pip.x - wrist.x)**2 + (pip.y - wrist.y)**2)

            if tip.y < pip.y and wrist_to_tip > wrist_to_pip:
                fingers.append(1)
            else:
                fingers.append(0)

        return fingers

    def detect_gesture(self, landmarks):
        """
        Detect gesture based on finger positions.
        Returns: 'rock', 'paper', 'scissors', or 'unknown'
        """
        fingers = self.count_fingers(landmarks)
        total_fingers = sum(fingers)

        # Debug output
        # print(f"Fingers: {fingers}, Total: {total_fingers}")

        # Rock: 0 fingers extended
        if total_fingers == 0:
            return 'rock'

        # Paper: all 5 fingers extended
        if total_fingers == 5:
            return 'paper'

        # Scissors: exactly 2 fingers (index and middle) extended
        # fingers = [thumb, index, middle, ring, pinky]
        if total_fingers == 2:
            if fingers[1] == 1 and fingers[2] == 1:
                return 'scissors'

        # Scissors with thumb (3 fingers total: thumb, index, middle)
        if total_fingers == 3:
            if fingers[0] == 1 and fingers[1] == 1 and fingers[2] == 1:
                return 'scissors'

        # Additional checks for rock (only thumb out counts as rock for RPS)
        if total_fingers == 1 and fingers[0] == 1:
            return 'rock'

        return 'unknown'

    def process_frame(self, frame):
        """Process a frame and return (annotated_frame, gesture)."""
        if not self.available:
            return frame, "Detector not available"

        # Convert BGR to RGB
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=frame_rgb)

        # Detect hands
        detection_result = self.detector.detect(mp_image)

        gesture = "No hand"

        if detection_result.hand_landmarks:
            for hand_landmarks in detection_result.hand_landmarks:
                # Draw landmarks
                self._draw_landmarks(frame, hand_landmarks)
                # Detect gesture
                gesture = self.detect_gesture(hand_landmarks)
                break  # Only process first hand

        # Add text overlay
        # Add background rectangle for text
        text = f"Gesture: {gesture.upper()}"
        (w, h), _ = cv2.getTextSize(text, cv2.FONT_HERSHEY_SIMPLEX, 1.2, 3)
        cv2.rectangle(frame, (10, 10), (w + 30, h + 30), (0, 0, 0), -1)
        cv2.putText(frame, text, (20, h + 15),
                   cv2.FONT_HERSHEY_SIMPLEX, 1.2, (0, 255, 0), 3)

        return frame, gesture

    def _draw_landmarks(self, image, hand_landmarks):
        """Draw hand landmarks on the image."""
        height, width = image.shape[:2]

        # Define connections (skeleton)
        connections = [
            (0, 1), (1, 2), (2, 3), (3, 4),      # Thumb
            (0, 5), (5, 6), (6, 7), (7, 8),      # Index
            (0, 9), (9, 10), (10, 11), (11, 12), # Middle
            (0, 13), (13, 14), (14, 15), (15, 16), # Ring
            (0, 17), (17, 18), (18, 19), (19, 20), # Pinky
            (5, 9), (9, 13), (13, 17)            # Palm
        ]

        # Draw connections
        for start_idx, end_idx in connections:
            start_pt = hand_landmarks[start_idx]
            end_pt = hand_landmarks[end_idx]
            start_point = (int(start_pt.x * width), int(start_pt.y * height))
            end_point = (int(end_pt.x * width), int(end_pt.y * height))
            cv2.line(image, start_point, end_point, (0, 255, 0), 2)

        # Draw landmark points
        for landmark in hand_landmarks:
            x = int(landmark.x * width)
            y = int(landmark.y * height)
            cv2.circle(image, (x, y), 5, (255, 0, 0), -1)


def test_stream_connection():
    """Test if we can connect to ESP32-CAM stream."""
    print(f"Testing connection to {ESP32_STREAM_URL}...")
    cap = cv2.VideoCapture(ESP32_STREAM_URL)

    # Set buffer size to reduce latency
    cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

    if cap.isOpened():
        ret, frame = cap.read()
        cap.release()
        if ret and frame is not None:
            print(f"Connection successful! Frame size: {frame.shape}")
            return True
        else:
            print("Connected but couldn't read frame")
            return False
    else:
        print("Failed to open connection")
        return False


def capture_frames():
    """Background thread that captures frames from ESP32-CAM."""
    global latest_frame, frame_count

    print(f"Connecting to ESP32-CAM at {ESP32_STREAM_URL}...")

    while True:
        cap = cv2.VideoCapture(ESP32_STREAM_URL)
        cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

        if not cap.isOpened():
            print("Failed to connect to ESP32-CAM. Retrying in 3 seconds...")
            time.sleep(3)
            continue

        print("Connected to ESP32-CAM! Capturing frames...")

        consecutive_failures = 0

        while True:
            ret, frame = cap.read()

            if ret and frame is not None:
                with frame_lock:
                    latest_frame = frame.copy()
                    frame_count += 1
                consecutive_failures = 0
            else:
                consecutive_failures += 1
                if consecutive_failures > 10:
                    print(f"Lost connection ({consecutive_failures} failures). Reconnecting...")
                    break

            time.sleep(0.01)  # Small delay to prevent CPU overload

        cap.release()
        time.sleep(1)


@app.route('/')
def index():
    """Serve the main webpage."""
    return render_template('index.html')


@app.route('/processed_feed')
def processed_feed():
    """Stream annotated video with gesture detection."""
    def generate():
        print("Client connected to video stream")

        while True:
            with frame_lock:
                if latest_frame is None:
                    time.sleep(0.1)
                    continue

                frame = latest_frame.copy()

            # Process frame for gesture detection
            if detector and detector.available:
                annotated_frame, gesture = detector.process_frame(frame)

                # Update global gesture
                with gesture_lock:
                    current_gesture = gesture
            else:
                annotated_frame = frame

            # Encode frame as JPEG
            ret, buffer = cv2.imencode('.jpg', annotated_frame, [cv2.IMWRITE_JPEG_QUALITY, 80])
            if not ret:
                continue

            frame_bytes = buffer.tobytes()

            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')

    return Response(stream_with_context(generate()),
                    mimetype='multipart/x-mixed-replace; boundary=frame')


@app.route('/gesture')
def get_gesture():
    """Return the current detected gesture as JSON."""
    with gesture_lock:
        return jsonify({'gesture': current_gesture})


@app.route('/status')
def status():
    """Return server status."""
    with frame_lock:
        frame_available = latest_frame is not None
    return jsonify({
        'connected': frame_available,
        'detector_ready': detector.available if detector else False,
        'frame_count': frame_count,
        'current_gesture': current_gesture
    })


if __name__ == '__main__':
    # Test connection first
    if not test_stream_connection():
        print("\n" + "="*50)
        print("WARNING: Could not connect to ESP32-CAM!")
        print("="*50)
        print("Make sure:")
        print("1. You are connected to ESP32-CAM WiFi")
        print("2. ESP32-CAM is powered on")
        print("3. ESP32-CAM streaming code is running")
        print("="*50 + "\n")

    # Initialize detector
    detector = GestureDetector()

    # Start frame capture thread
    capture_thread = threading.Thread(target=capture_frames, daemon=True)
    capture_thread.start()

    # Wait for first frame
    print("Waiting for first frame...")
    for _ in range(50):  # Wait up to 5 seconds
        with frame_lock:
            if latest_frame is not None:
                print(f"First frame received! Size: {latest_frame.shape}")
                break
        time.sleep(0.1)

    # Run Flask server
    print("\n" + "="*50)
    print("Gesture Detection Server Started!")
    print("="*50)
    print("Open in browser: http://localhost:5000")
    print("Press Ctrl+C to stop")
    print("="*50 + "\n")

    app.run(host='0.0.0.0', port=5000, debug=False, threaded=True)
