#include "GameOver.h"
#include "GameConfig.h"

// ===============================
// 💀 LAYAR GAME OVER
// ===============================

GameOver::GameOver() {
    Reset();
}

// Reset skor dan status ke nilai awal
void GameOver::Reset() {
    score = 0;
    returnToMenu = false;
}

// Simpan skor akhir untuk ditampilkan di Draw()
void GameOver::SetScore(int s) {
    score = s;
}

// Tunggu input ENTER untuk kembali ke menu utama
void GameOver::Update() {
    if (IsKeyPressed(KEY_ENTER)) {
        returnToMenu = true;
    }
}

// Render layar game over: judul merah, skor, petunjuk
void GameOver::Draw() {
    int centerX = Config::screenWidth / 2;
    int yStart = 100;

    // Judul GAME OVER
    const char* title = "GAME OVER";
    int titleW = MeasureText(title, 40);
    DrawText(title, centerX - titleW/2, yStart, 40, RED);

    // Tampilkan skor akhir
    const char* scoreText = TextFormat("Score Kamu: %d", score);
    int scoreW = MeasureText(scoreText, 20);
    DrawText(scoreText, centerX - scoreW/2, yStart + 80, 20, WHITE);

    // Petunjuk kembali
    const char* btn = "Tekan ENTER untuk kembali";
    int btnW = MeasureText(btn, 20);
    DrawText(btn, centerX - btnW/2, yStart + 140, 20, GRAY);
}

bool GameOver::ShouldReturnToMenu() const {
    return returnToMenu;
}
