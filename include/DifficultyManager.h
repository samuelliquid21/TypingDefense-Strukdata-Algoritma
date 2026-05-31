#pragma once

// Struct untuk mengatur kesulitan tiap waktu berlalu
struct DifficultyManager {

    // ==== FUNGSI VARIABEL YANG DIPAKAI DI MODUL LAIN ====

    float counter{0.0f};
    void updateTime(); // update waktu              
    void resetTime(); // reset waktu
    int  getAsteroidTier() const; // menentukan tier asteroid di asteroid pool (untuk spawning)
    int  getAsteroidCountForSpecialSpawn() const; // FIX: ubah menjadi bool saja
};