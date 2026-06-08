#include "Game.h"

// Entry point game TypingDefense
// Membuat instance Game dan memulai game loop utama
int main() {
    Game game;      // Inisialisasi seluruh subsistem game
    game.Run();     // Game loop: Update() -> Draw() setiap frame
    return 0;       // Keluar setelah window ditutup
}
