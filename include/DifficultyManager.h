#pragma once

struct DifficultyManager {
    float counter{0.0f};

    void updateTime();
    void resetTime();
    int  getAsteroidTier() const;
    int  getAsteroidCountForSpecialSpawn() const;
};