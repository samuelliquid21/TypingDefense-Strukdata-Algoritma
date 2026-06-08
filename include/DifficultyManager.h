#pragma once

// Struct pengatur tingkat kesulitan berdasarkan waktu permainan yang telah berlalu
struct DifficultyManager {

    float counter{0.0f}; // Waktu permainan dalam detik

    void updateTime();                // Update waktu dengan GetFrameTime()
    void resetTime();                 // Reset waktu ke 0
    int  getAsteroidTier() const;     // Tentukan tier asteroid berdasarkan waktu (1-6)
    int  getAsteroidCountForSpecialSpawn() const; // Jumlah spawn khusus berdasarkan waktu
};
