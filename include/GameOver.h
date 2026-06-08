#pragma once
#include "raylib.h"
#include <string>

// Class untuk state Game Over: menampilkan skor akhir dan opsi kembali ke menu
class GameOver {
private:
    int score;           // Skor akhir player yang akan ditampilkan
    bool returnToMenu;   // Flag untuk memberi sinyal ke state machine agar kembali ke menu

    static constexpr int screenWidth = 1080;
    static constexpr int screenHeight = 720;

public:
    GameOver();           // Constructor: panggil Reset()
    void Reset();         // Reset state ke default
    void SetScore(int s); // Set skor yang akan ditampilkan
    void Update();        // Handle input: ENTER untuk kembali ke menu
    void Draw();          // Render tampilan Game Over (skor, instruksi)
    bool ShouldReturnToMenu() const; // Cek apakah player memilih untuk kembali
};
