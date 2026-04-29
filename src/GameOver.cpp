#include "GameOver.h"

GameOver::GameOver() {
    Reset();
}

void GameOver::Reset() {
    score = 0;
    highestScore = 0;
    playerName = "";
    isSaved = false;
    returnToMenu = false;
}

void GameOver::SetScore(int s) {
    score = s;
}

void GameOver::SetHighestScore(int hs) {
    highestScore = hs;
}

void GameOver::Update() {
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (playerName.length() < 15)) {
            playerName.push_back((char)key);
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !playerName.empty()) {
        playerName.pop_back();
    }

    if (IsKeyPressed(KEY_ENTER) && !playerName.empty()) {
        if (!isSaved) {
            auto j = ScoreManager::Load("./data/data.json");
            ScoreManager::InsertOrUpdate(j, playerName, score);
            ScoreManager::Save("./data/data.json", j);
            highestScore = ScoreManager::GetHighest(j);
            isSaved = true;
        }
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

    if (!isSaved) {
        const char* prompt = "Masukkan Nama:";
        int promptW = MeasureText(prompt, 20);
        DrawText(prompt, centerX - promptW/2, yStart + 140, 20, YELLOW);

        const char* name = playerName.c_str();
        int nameW = MeasureText(name, 20);
        DrawText(name, centerX - nameW/2, yStart + 170, 20, WHITE);
        DrawRectangleLines(centerX - 100, yStart + 170, 200, 25, WHITE);
    } else {
        const char* highText = TextFormat("Skor Tertinggi: %d", highestScore);
        int highW = MeasureText(highText, 20);
        DrawText(highText, centerX - highW/2, yStart + 140, 20, GREEN);
    }

    const char* btn = isSaved ? "Tekan ENTER untuk kembali" : "Tekan ENTER untuk simpan";
    int btnW = MeasureText(btn, 20);
    DrawText(btn, centerX - btnW/2, yStart + 230, 20, GRAY);
}

bool GameOver::IsSaved() const {
    return isSaved;
}

bool GameOver::ShouldReturnToMenu() const {
    return returnToMenu;
}