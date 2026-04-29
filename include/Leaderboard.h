#pragma once
#include "raylib.h"
#include <string>
#include <vector>

namespace LeaderboardSystem {

    struct PlayerData {
        std::string name;       // ← HARUS "name" bukan "username"
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

} // namespace LeaderboardSystem