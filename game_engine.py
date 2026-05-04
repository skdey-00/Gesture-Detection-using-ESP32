"""
Rock Paper Scissors Game Engine

Handles game logic, scoring, and winner determination
for the gesture detection game.
"""

import requests
from threading import Lock


class GameEngine:
    """
    Game engine for Rock Paper Scissors.
    Manages scoring, move comparison, and ESP32 player communication.
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

    def __init__(self, esp32_player_url="http://192.168.4.2:81"):
        """
        Initialize the game engine.

        Args:
            esp32_player_url: URL of the ESP32 player HTTP endpoint
        """
        self.esp32_url = esp32_player_url
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
        """Test if ESP32 player is reachable."""
        try:
            response = requests.get(f"{self.esp32_url}/status", timeout=3)
            self.esp32_connected = response.status_code == 200
            print(f"ESP32 Player connection: {'OK' if self.esp32_connected else 'FAILED'}")
        except Exception as e:
            self.esp32_connected = False
            print(f"ESP32 Player connection failed: {e}")

    def get_esp32_move(self):
        """
        Get a random move from the ESP32 player.

        Returns:
            str: 'rock', 'paper', or 'scissors', or 'error' if failed
        """
        try:
            response = requests.get(f"{self.esp32_url}/move", timeout=5)
            if response.status_code == 200:
                data = response.json()
                move = data.get("move", "").lower()
                if move in self.VALID_MOVES:
                    self.esp32_connected = True
                    return move
            return "error"
        except Exception as e:
            print(f"Error getting ESP32 move: {e}")
            self.esp32_connected = False
            return "error"

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
        with self.lock:
            # Normalize player gesture
            player_move = player_gesture.lower() if isinstance(player_gesture, str) else "unknown"

            # Get ESP32's move
            esp32_move = self.get_esp32_move()

            # Handle ESP32 connection error
            if esp32_move == "error":
                # Fallback: generate random move locally
                import random
                esp32_move = random.choice(list(self.VALID_MOVES))
                print("Using local random move (ESP32 unreachable)")

            # Determine winner
            result = self.determine_winner(player_move, esp32_move)

            # Generate message
            if result == self.UNKNOWN:
                message = "Unable to detect valid gesture"
            elif result == self.TIE:
                message = "It's a tie!"
            elif result == self.PLAYER_WINS:
                message = f"{player_move.capitalize()} beats {esp32_move}! You win!"
            else:
                message = f"{esp32_move.capitalize()} beats {player_move}! ESP32 wins!"

            # Update scores
            if result in self.scores:
                self.scores[result] += 1

            # Record history
            round_record = {
                "player_move": player_move,
                "esp32_move": esp32_move,
                "result": result,
                "message": message
            }
            self.history.append(round_record)

            # Return results
            return {
                "player_move": player_move,
                "esp32_move": esp32_move,
                "result": result,
                "message": message,
                "scores": self.get_scores(),
                "esp32_connected": self.esp32_connected
            }

    def get_scores(self):
        """Get current scores."""
        with self.lock:
            return self.scores.copy()

    def reset_scores(self):
        """Reset all scores to zero."""
        with self.lock:
            self.scores = {"player": 0, "esp32": 0, "tie": 0}
            self.history = []

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
