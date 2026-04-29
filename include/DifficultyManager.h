#pragma once

// Struct untuk mengatur kesulitan tiap waktu berlalu
struct DifficultyManager {
    float counter{0.0f};

    void updateTime();                             
    void resetTime();
    int  getAsteroidTier() const;
    int  getAsteroidCountForSpecialSpawn() const;
};