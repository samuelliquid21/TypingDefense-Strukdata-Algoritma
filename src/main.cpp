#include "Game.h"

// ===============================
// ▶️ ENTRY POINT
// ===============================

// Entry point game TypingDefense — Cosmic Keypad
// Membuat instance Game dan memulai game loop utama (60 FPS)
int main() {
    Game game;      // Inisialisasi seluruh subsistem game
    game.Run();     // Game loop: Update() -> Draw() setiap frame
    return 0;       // Keluar setelah window ditutup
}
