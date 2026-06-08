#include "GameOver.h"

GameOver::GameOver() {
    Reset();
}

void GameOver::Reset() {
    score = 0;
    returnToMenu = false;  // Belum memilih kembali
}

void GameOver::SetScore(int s) {
    score = s; // Simpan skor akhir untuk ditampilkan di Draw()
}

void GameOver::Update() {
    // Tunggu input ENTER untuk kembali ke menu utama
    if (IsKeyPressed(KEY_ENTER)) {
        returnToMenu = true; // Sinyal ke state machine
    }
}

void GameOver::Draw() {
    int centerX = screenWidth / 2;
    int yStart = 100;

    // Judul "GAME OVER" di bagian atas dengan warna merah
    const char* title = "GAME OVER";
    int titleW = MeasureText(title, 40);
    DrawText(title, centerX - titleW/2, yStart, 40, RED);

    // Tampilkan skor akhir player
    const char* scoreText = TextFormat("Score Kamu: %d", score);
    int scoreW = MeasureText(scoreText, 20);
    DrawText(scoreText, centerX - scoreW/2, yStart + 80, 20, WHITE);

    // Petunjuk untuk melanjutkan
    const char* btn = "Tekan ENTER untuk kembali";
    int btnW = MeasureText(btn, 20);
    DrawText(btn, centerX - btnW/2, yStart + 140, 20, GRAY);
}

bool GameOver::ShouldReturnToMenu() const {
    return returnToMenu;
}
