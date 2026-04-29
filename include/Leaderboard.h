#pragma once
#include "raylib.h"
#include <string>
#include <vector>

// namespace untuk state fitur leaderboard

namespace LeaderboardSystem {

    struct PlayerData {
        std::string name;       
        int score;
        float accuracy;
        int wordsTyped;
        int enemiesDefeated;
        float survivalTime;
        int rank;
    };

    void Init();
    void Update(bool& backToMenu);
    void Draw();
    void Unload();

} 