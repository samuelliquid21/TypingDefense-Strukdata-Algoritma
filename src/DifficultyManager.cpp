#include "DifficultyManager.h"
#include "raylib.h"

// ===============================
// 📈 MANAGER KESULITAN
// ===============================

// Update akumulasi waktu berdasarkan frame time
void DifficultyManager::updateTime() {
    counter += GetFrameTime();
}

// Reset waktu ke 0 (dipanggil saat game restart)
void DifficultyManager::resetTime() {
    counter = 0.0f;
}

// Tentukan tier asteroid (1-6) berdasarkan waktu yang telah berlalu
// Tier atas naik 1 setiap ~20 detik (0.05 per detik), maksimal 6
// Tier bawah mulai naik setelah 60 detik, maksimal 4
int DifficultyManager::getAsteroidTier() const {
    // Batas atas: naik dari 2 hingga 6 seiring waktu
    int upper = 2 + static_cast<int>(counter * 0.05f);
    if (upper > 6) upper = 6;

    // Batas bawah: tetap 1 selama 60 detik pertama, lalu naik hingga 4
    int lower = 1;
    if (counter > 60.0f) {
        lower = 1 + static_cast<int>((counter - 60.0f) * 0.05f);
        if (lower > 4) lower = 4;
    }

    // Return tier random di antara lower dan upper
    return GetRandomValue(lower, upper);
}

// Tentukan jumlah asteroid untuk special spawn, meningkat seiring waktu
int DifficultyManager::getAsteroidCountForSpecialSpawn() const {
    if (counter <= 60.0f)
        return 5; // Default 5 untuk 60 detik pertama

    // Naik 1 setiap 60 detik, maksimal 15
    int count = static_cast<int>(counter / 60.0f) + 5;
    if (count > 15) count = 15;

    return count;
}
