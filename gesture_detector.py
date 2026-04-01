"""
Rock Paper Scissors Gesture Detector
Uses MediaPipe Tasks API for hand landmark detection and gesture classification.
"""

import cv2
import mediapipe as mp
import numpy as np
import os
from pathlib import Path
import time


class GestureDetector:
    def __init__(self):
        # Using the new MediaPipe Tasks API (version 0.10+)
        from mediapipe.tasks.python.vision import HandLandmarker, HandLandmarkerOptions
        from mediapipe.tasks.python import BaseOptions

        # Create hand landmarker
        base_options = BaseOptions(model_asset_path='hand_landmarker.task')
        options = HandLandmarkerOptions(base_options=base_options,
                                        num_hands=2)
        self.detector = HandLandmarker.create_from_options(options)

    def count_fingers(self, landmarks):
        """
        Count extended fingers based on landmarks.
        """
        # Finger tip and PIP indices for MediaPipe
        # Thumb: 4 (tip), 3 (ip), 2 (mcp)
        # Index: 8 (tip), 6 (pip)
        # Middle: 12 (tip), 10 (pip)
        # Ring: 16 (tip), 14 (pip)
        # Pinky: 20 (tip), 18 (pip)

        fingers = []

        # Thumb - compare x position (depends on hand side)
        # Simple check: compare tip to IP joint x position
        thumb_tip = landmarks[4]
        thumb_ip = landmarks[3]
        if abs(thumb_tip.x - thumb_ip.x) > 0.05:  # Extended if x diff is significant
            fingers.append(1)
        else:
            fingers.append(0)

        # Other fingers - check if tip is above PIP (lower y value = higher)
        for tip_idx, pip_idx in [(8, 6), (12, 10), (16, 14), (20, 18)]:
            if landmarks[tip_idx].y < landmarks[pip_idx].y:
                fingers.append(1)  # Extended
            else:
                fingers.append(0)  # Closed

        return fingers

    def detect_gesture(self, landmarks):
        """
        Detect gesture based on finger positions.
        Returns: 'rock', 'paper', 'scissors', or 'unknown'
        """
        fingers = self.count_fingers(landmarks)
        total_fingers = sum(fingers)

        # Rock: all fingers closed (or only thumb)
        if total_fingers == 0:
            return 'rock'
        if total_fingers == 1 and fingers[0] == 1:
            return 'rock'

        # Paper: all 5 fingers extended
        if total_fingers == 5:
            return 'paper'

        # Scissors: index and middle extended, ring and pinky closed
        # fingers = [thumb, index, middle, ring, pinky]
        if fingers[1] == 1 and fingers[2] == 1 and fingers[3] == 0 and fingers[4] == 0:
            return 'scissors'

        # Also check for scissors with thumb extended
        if fingers[0] == 1 and fingers[1] == 1 and fingers[2] == 1 and fingers[3] == 0 and fingers[4] == 0:
            return 'scissors'

        return 'unknown'

    def process_image(self, image_path, display=True):
        """
        Process a single image and detect gesture.
        """
        image = cv2.imread(str(image_path))
        if image is None:
            print(f"Could not read image: {image_path}")
            return None

        # Convert BGR to RGB
        image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=image_rgb)

        # Detect hands
        detection_result = self.detector.detect(mp_image)

        gesture = "No hand detected"

        if detection_result.hand_landmarks:
            for hand_landmarks in detection_result.hand_landmarks:
                # Draw landmarks
                self.draw_landmarks(image, hand_landmarks)

                # Detect gesture
                gesture = self.detect_gesture(hand_landmarks)

                # Display gesture on image
                cv2.putText(image, gesture, (50, 50),
                           cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 255, 0), 3)

        if display:
            # Display result
            cv2.imshow('Gesture Detection', image)
            cv2.waitKey(0)
            cv2.destroyAllWindows()

        return gesture

    def draw_landmarks(self, image, landmarks):
        """Draw hand landmarks on the image."""
        height, width = image.shape[:2]

        # Draw connections
        connections = [
            (0, 1), (1, 2), (2, 3), (3, 4),  # Thumb
            (0, 5), (5, 6), (6, 7), (7, 8),  # Index
            (0, 9), (9, 10), (10, 11), (11, 12),  # Middle
            (0, 13), (13, 14), (14, 15), (15, 16),  # Ring
            (0, 17), (17, 18), (18, 19), (19, 20),  # Pinky
            (5, 9), (9, 13), (13, 17)  # Palm
        ]

        for start_idx, end_idx in connections:
            start_point = (int(landmarks[start_idx].x * width),
                          int(landmarks[start_idx].y * height))
            end_point = (int(landmarks[end_idx].x * width),
                        int(landmarks[end_idx].y * height))
            cv2.line(image, start_point, end_point, (0, 255, 0), 2)

        # Draw landmarks
        for landmark in landmarks:
            x = int(landmark.x * width)
            y = int(landmark.y * height)
            cv2.circle(image, (x, y), 5, (255, 0, 0), -1)

    def process_webcam(self):
        """
        Real-time gesture detection from webcam.
        """
        cap = cv2.VideoCapture(0)

        if not cap.isOpened():
            print("Could not open webcam")
            return

        print("Webcam started. Press 'q' to quit.")

        while cap.isOpened():
            ret, frame = cap.read()
            if not ret:
                break

            # Convert BGR to RGB
            frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

            # Create MP Image
            mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=frame_rgb)

            # Detect hands
            detection_result = self.detector.detect(mp_image)

            gesture = "No hand detected"

            if detection_result.hand_landmarks:
                for hand_landmarks in detection_result.hand_landmarks:
                    # Draw landmarks
                    self.draw_landmarks(frame, hand_landmarks)

                    # Detect gesture
                    gesture = self.detect_gesture(hand_landmarks)

            # Display gesture on frame
            cv2.putText(frame, f"Gesture: {gesture}", (10, 50),
                       cv2.FONT_HERSHEY_SIMPLEX, 1.5, (0, 255, 0), 3)
            cv2.putText(frame, "Press 'q' to quit", (10, frame.shape[0] - 20),
                       cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

            cv2.imshow('Rock Paper Scissors Detector', frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        cap.release()
        cv2.destroyAllWindows()


def download_model():
    """Download the required hand landmarker model."""
    import urllib.request

    model_url = "https://storage.googleapis.com/mediapipe-models/hand_landmarker/hand_landmarker/float16/1/hand_landmarker.task"
    model_path = "hand_landmarker.task"

    if os.path.exists(model_path):
        print("Model already exists.")
        return True

    print("Downloading hand landmarker model...")
    try:
        urllib.request.urlretrieve(model_url, model_path)
        print("Model downloaded successfully!")
        return True
    except Exception as e:
        print(f"Error downloading model: {e}")
        return False


def main():
    # First, ensure model exists
    if not os.path.exists('hand_landmarker.task'):
        print("Hand landmarker model not found.")
        if not download_model():
            print("Could not download model. Please download manually from:")
            print("https://storage.googleapis.com/mediapipe-models/hand_landmarker/hand_landmarker/float16/1/hand_landmarker.task")
            return

    try:
        detector = GestureDetector()
    except Exception as e:
        print(f"Error initializing detector: {e}")
        return

    print("\nRock Paper Scissors Gesture Detector")
    print("="*50)
    print("1. Real-time webcam detection")
    print("2. Detect single image")

    choice = input("\nEnter your choice (1/2): ").strip()

    if choice == '1':
        print("\nStarting webcam... Press 'q' to quit")
        detector.process_webcam()

    elif choice == '2':
        img_path = input("Enter image path: ").strip()
        if os.path.exists(img_path):
            gesture = detector.process_image(img_path, display=True)
            print(f"\nDetected gesture: {gesture}")
        else:
            print("Image not found!")

    else:
        print("Invalid choice")


if __name__ == "__main__":
    main()
