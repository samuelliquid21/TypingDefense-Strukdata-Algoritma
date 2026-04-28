#include "DifficultyManager.h"
#include "raylib.h"

// meng update waktu yang telah berlalu
void DifficultyManager::updateTime() {
    counter += GetFrameTime();
}

// mereset waktu yang telah berlalu
void DifficultyManager::resetTime() {
    counter = 0.0f;
}

// mendapatkan tier asteroid berdasarkan waktu yang telah berlalu
int DifficultyManager::getAsteroidTier() const {
    int upper = 2 + static_cast<int>(counter * 0.05f);
    if (upper > 6) upper = 6;

    int lower = 1;
    if (counter > 60.0f) {
        lower = 1 + static_cast<int>((counter - 60.0f) * 0.05f);
        if (lower > 4) lower = 4;
    }

    return GetRandomValue(lower, upper);
}

// mendapatkan jumlah asteroid untuk spawn khusus berdasarkan waktu yang telah berlalu
int DifficultyManager::getAsteroidCountForSpecialSpawn() const {
    if (counter <= 60.0f)
        return 8;

    int count = static_cast<int>(counter / 60.0f) + 8;
    if (count > 15) count = 15;    

    return count;
}