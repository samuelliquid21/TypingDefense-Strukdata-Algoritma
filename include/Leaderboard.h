#pragma once
#include "raylib.h"
#include <string>
#include <vector>

// Namespace untuk state dan rendering fitur leaderboard
// Berisi data player, logika sorting, animasi masuk/keluar, serta input navigation

namespace LeaderboardSystem {

    // Data satu entri leaderboard (disimpan dari JSON dan ditampilkan)
    struct PlayerData {
        std::string name;       
        int score;
        float accuracy;
        int wordsTyped;
        int enemiesDefeated;
        float survivalTime;
        int rank;               // Peringkat (diisi otomatis setelah sorting)
    };

    void Init();                // Inisialisasi: reset state, load audio & data dari JSON
    void Update(bool& backToMenu); // Update frame: input, animasi, musik
    void Draw();                // Render leaderboard dengan animasi entry/exit
    void Unload();              // Cleanup resources (audio)

} 
