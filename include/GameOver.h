#pragma once
#include "raylib.h"
#include <string>

class GameOver {
private:
    int score;
    bool returnToMenu;

    static constexpr int screenWidth = 1080;
    static constexpr int screenHeight = 720;

public:
    GameOver();
    void Reset();
    void SetScore(int s);
    void Update();
    void Draw();
    bool ShouldReturnToMenu() const;
};
