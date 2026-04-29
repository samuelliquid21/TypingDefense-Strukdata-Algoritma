#pragma once
#include "raylib.h"
#include <string>
#include <ScoreManager.h>

class GameOver {
private:
    int score;
    int highestScore;
    std::string playerName;
    bool isSaved;
    bool returnToMenu;

    static constexpr int screenWidth = 1080;
    static constexpr int screenHeight = 720;

public:
    GameOver();
    void Reset();
    void SetScore(int s);
    void SetHighestScore(int hs);
    void Update();
    void Draw();
    bool IsSaved() const;
    bool ShouldReturnToMenu() const;
};