"""
Rock Paper Scissors Game Engine - UART Version

Handles game logic, scoring, and winner determination
for the gesture detection game with ESP32 UART communication.

This version communicates with ESP32-CAM which handles
UART communication with ESP32 #2 (Servo controller).
"""

import requests
from threading import Lock


class GameEngine:
    """
    Game engine for Rock Paper Scissors with UART communication.
    The ESP32-CAM handles UART communication with the servo ESP32.
    """

    # Game outcomes
    PLAYER_WINS = "player"
    ESP32_WINS = "esp32"
    TIE = "tie"
    UNKNOWN = "unknown"

    # Valid moves
    VALID_MOVES = {"rock", "paper", "scissors"}

    # Winning combinations (key beats value)
    WINNING_COMBOS = {
        "rock": "scissors",      # Rock crushes scissors
        "scissors": "paper",     # Scissors cuts paper
        "paper": "rock"          # Paper covers rock
    }

    def __init__(self, esp32_cam_url="http://192.168.4.1"):
        """
        Initialize the game engine.

        Args:
            esp32_cam_url: URL of the ESP32-CAM HTTP endpoint
        """
        self.esp32_cam_url = esp32_cam_url
        self.scores = {
            "player": 0,
            "esp32": 0,
            "tie": 0
        }
        self.history = []
        self.lock = Lock()
        self.esp32_connected = False

        # Test ESP32 connection on init
        self._test_esp32_connection()

    def _test_esp32_connection(self):
        """Test if ESP32-CAM is reachable."""
        try:
            response = requests.get(f"{self.esp32_cam_url}/status", timeout=3)
            self.esp32_connected = response.status_code == 200
            print(f"ESP32-CAM connection: {'OK' if self.esp32_connected else 'FAILED'}")
        except Exception as e:
            self.esp32_connected = False
            print(f"ESP32-CAM connection failed: {e}")

    def get_esp32_move(self, player_move):
        """
        Request a move from ESP32-CAM.
        The ESP32-CAM communicates with ESP32 #2 via UART
        and returns the move.

        Args:
            player_move: The player's detected gesture

        Returns:
            dict: Response containing esp32_move, result, message, scores
        """
        try:
            # Send play request to ESP32-CAM
            # ESP32-CAM will:
            # 1. Send PLAY command to ESP32 #2 via UART
            # 2. Receive move from ESP32 #2 via UART
            # 3. Determine winner
            # 4. Update scores
            # 5. Return result
            response = requests.get(
                f"{self.esp32_cam_url}/play",
                params={"move": player_move},
                timeout=10  # Longer timeout for UART communication
            )

            if response.status_code == 200:
                data = response.json()
                self.esp32_connected = data.get("esp32_connected", True)

                # Update local scores
                if "scores" in data:
                    with self.lock:
                        self.scores = data["scores"]

                # Record history
                with self.lock:
                    self.history.append({
                        "player_move": data.get("player_move", player_move),
                        "esp32_move": data.get("esp32_move", "unknown"),
                        "result": data.get("result", "unknown"),
                        "message": data.get("message", "")
                    })

                return data
            else:
                print(f"ESP32-CAM returned error: {response.status_code}")
                self.esp32_connected = False
                return None

        except Exception as e:
            print(f"Error getting ESP32 move: {e}")
            self.esp32_connected = False
            return None

    def determine_winner(self, player_move, esp32_move):
        """
        Determine the winner of a round.

        Args:
            player_move: Player's gesture ('rock', 'paper', 'scissors', or 'unknown')
            esp32_move: ESP32's move ('rock', 'paper', 'scissors')

        Returns:
            str: 'player', 'esp32', 'tie', or 'unknown'
        """
        # Check for invalid player gesture
        if player_move not in self.VALID_MOVES:
            return self.UNKNOWN

        # Check for invalid ESP32 move
        if esp32_move not in self.VALID_MOVES:
            return self.UNKNOWN

        # Check for tie
        if player_move == esp32_move:
            return self.TIE

        # Check if player wins
        if self.WINNING_COMBOS[player_move] == esp32_move:
            return self.PLAYER_WINS

        # Otherwise ESP32 wins
        return self.ESP32_WINS

    def play_round(self, player_gesture):
        """
        Play a complete round of Rock Paper Scissors.

        Args:
            player_gesture: The gesture detected from camera ('rock', 'paper', 'scissors', 'unknown', 'No hand')

        Returns:
            dict: Round results containing:
                - player_move: Player's gesture
                - esp32_move: ESP32's move
                - result: 'player', 'esp32', 'tie', or 'unknown'
                - message: Human-readable result message
                - scores: Current scores
        """
        # Normalize player gesture
        player_move = player_gesture.lower() if isinstance(player_gesture, str) else "unknown"

        # Check for valid player move
        if player_move not in self.VALID_MOVES:
            return {
                "player_move": player_move,
                "esp32_move": "none",
                "result": self.UNKNOWN,
                "message": "Unable to detect valid gesture",
                "scores": self.get_scores(),
                "esp32_connected": self.esp32_connected
            }

        # Get ESP32's move via ESP32-CAM
        esp32_response = self.get_esp32_move(player_move)

        if esp32_response:
            return esp32_response
        else:
            # Fallback: generate random move locally
            import random
            esp32_move = random.choice(list(self.VALID_MOVES))
            result = self.determine_winner(player_move, esp32_move)

            # Generate message
            if result == self.UNKNOWN:
                message = "Unable to determine winner"
            elif result == self.TIE:
                message = "It's a tie!"
            elif result == self.PLAYER_WINS:
                message = f"{player_move.capitalize()} beats {esp32_move}! You win!"
            else:
                message = f"{esp32_move.capitalize()} beats {player_move}! ESP32 wins!"

            # Update scores
            with self.lock:
                if result in self.scores:
                    self.scores[result] += 1

                # Record history
                self.history.append({
                    "player_move": player_move,
                    "esp32_move": esp32_move,
                    "result": result,
                    "message": message
                })

            return {
                "player_move": player_move,
                "esp32_move": esp32_move,
                "result": result,
                "message": message + " (Local fallback - ESP32 unreachable)",
                "scores": self.get_scores(),
                "esp32_connected": False
            }

    def get_scores(self):
        """Get current scores."""
        with self.lock:
            return self.scores.copy()

    def reset_scores(self):
        """Reset all scores to zero."""
        # Reset local scores
        with self.lock:
            self.scores = {"player": 0, "esp32": 0, "tie": 0}
            self.history = []

        # Also reset scores on ESP32-CAM
        try:
            requests.post(f"{self.esp32_cam_url}/reset", timeout=5)
        except:
            pass

    def get_history(self, limit=10):
        """
        Get game history.

        Args:
            limit: Maximum number of recent rounds to return

        Returns:
            list: Recent round records
        """
        with self.lock:
            return self.history[-limit:]


# Emoji representations for moves
MOVE_EMOJIS = {
    "rock": "✊",
    "paper": "✋",
    "scissors": "✌️",
    "unknown": "❓",
    "no hand": "👋"
}


def get_move_emoji(move):
    """Get emoji for a move."""
    return MOVE_EMOJIS.get(move.lower(), "❓")
