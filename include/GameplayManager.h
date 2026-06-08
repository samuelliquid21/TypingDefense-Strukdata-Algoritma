#pragma once

#include "SpaceShip.h"
#include "AsteroidManager.h"
#include "Combostack.h"
#include "ShieldSkill.h"
#include "BombSkill.h"
#include "raylib.h"
#include <functional>

// State mesin untuk sistem typing: mencari target vs sedang mengetik target
enum typingState {
    SEARCH_FOR_TARGET,   // Mencari asteroid baru untuk ditarget
    TARGET_LOCKED        // Sedang mengetik kata pada asteroid yang sudah ditarget
};

// Type alias untuk callback function pointer dan std::function
using ScoreCallback = void(*)(int score, int multiplier);              // Dipanggil saat skor berubah
using AsteroidDestroyedCallback = std::function<void(const std::string& word)>;  // Dipanggil saat asteroid hancur

// Kelas utama pengatur seluruh gameplay: skor, combo, asteroid, spaceship, skill, dan state typing
class GameplayManager
{
private:
    SpaceShip spaceship;                                    // Objek spaceship pemain
    AsteroidManager asteroidManager;                        // Pengelola seluruh asteroid (pool, shower, event queue)
    ComboStack comboStack;                                  // Stack untuk combo multiplier (max 6 level)
    ShieldSkill shieldSkill;                                 // Skill perisai pelindung pemain
    BombSkill bombSkill;                                     // Skill bom shockwave area
    typingState state = typingState::SEARCH_FOR_TARGET;     // State typing saat ini
    Asteroid* currentTarget = nullptr;                      // Pointer ke asteroid yang sedang diketik
    int wordsCompleted = 0;                                 // Counter kata selesai untuk naikkan combo
    bool wasPreviousKeyWrong = false;                       // Flag untuk mencegah error sound berulang

    ScoreCallback onScoreChanged = nullptr;                     // Callback saat skor berubah (nullable)
    AsteroidDestroyedCallback onAsteroidDestroyed = nullptr;    // Callback saat asteroid hancur (nullable)

    // Sound effect
    Sound laser;      // Suara laser saat mengetik benar
    Sound error;      // Suara error saat salah ketik
    Sound gameover;   // Suara game over

public:
    int score = 0;   // Skor pemain saat ini

    GameplayManager() = default; // Constructor default
    ~GameplayManager();          // Destructor: unload semua sound

    void AddScore(int points);                      // Tambah skor dengan multiplier combo stack
    void AddScore(int basePoints, int multiplier);  // Tambah skor dengan multiplier kustom + callback

    void SetScoreCallback(ScoreCallback callback);                 // Daftarkan callback perubahan skor
    void SetAsteroidDestroyedCallback(AsteroidDestroyedCallback callback); // Daftarkan callback asteroid hancur

    void textureInit(); // Inisialisasi texture spaceship dan load sound effect

    bool isHit(); // Cek tabrakan asteroid dengan player (shield atau tidak)

    void update(float deltaTime);   // Update utama: state machine typing, skill, input
    void draw();                    // Render semua objek game + HUD
    void reset();                   // Reset seluruh state ke kondisi awal permainan
};
