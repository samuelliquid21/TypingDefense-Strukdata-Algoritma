#include "GameOver.h"

GameOver::GameOver() {
    Reset();
}

void GameOver::Reset() {
    score = 0;
    returnToMenu = false;
}

void GameOver::SetScore(int s) {
    score = s;
}

void GameOver::Update() {
    if (IsKeyPressed(KEY_ENTER)) {
        returnToMenu = true;
    }
}

void GameOver::Draw() {
    int centerX = screenWidth / 2;
    int yStart = 100;

    const char* title = "GAME OVER";
    int titleW = MeasureText(title, 40);
    DrawText(title, centerX - titleW/2, yStart, 40, RED);

    const char* scoreText = TextFormat("Score Kamu: %d", score);
    int scoreW = MeasureText(scoreText, 20);
    DrawText(scoreText, centerX - scoreW/2, yStart + 80, 20, WHITE);

    const char* btn = "Tekan ENTER untuk kembali";
    int btnW = MeasureText(btn, 20);
    DrawText(btn, centerX - btnW/2, yStart + 140, 20, GRAY);
}

bool GameOver::ShouldReturnToMenu() const {
    return returnToMenu;
}
