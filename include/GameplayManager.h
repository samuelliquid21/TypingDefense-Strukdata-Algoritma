#pragma once

#include "SpaceShip.h"
#include "AsteroidManager.h"
#include "Combostack.h"
#include "ShieldSkill.h"
#include "BombSkill.h"
#include "raylib.h"
#include <functional>

enum typingState {
    SEARCH_FOR_TARGET,  
    TARGET_LOCKED       
};

// Type alias untuk function pointer callbacks
using ScoreCallback = void(*)(int score, int multiplier);  
using AsteroidDestroyedCallback = std::function<void(const std::string& word)>;  

// Kelas utama untuk manajemen gameplay (score, combo, asteroid, spaceship)
class GameplayManager
{
private:
    SpaceShip spaceship;                                    // Player spaceship
    AsteroidManager asteroidManager;                        // Manajer asteroid
    ComboStack comboStack;                                  // Stack untuk combo multiplier
    ShieldSkill shieldSkill;                                 // Skill shield player
    BombSkill bombSkill;                                     // Skill bomb shockwave
    typingState state = typingState::SEARCH_FOR_TARGET;     // State typing saat ini
    Asteroid* currentTarget = nullptr;                      // Pointer ke asteroid yang sedang ditarget
    int wordsCompleted = 0;                                 // Counter kata yang telah diselesaikan
    bool wasPreviousKeyWrong = false;                       // Flag untuk key salah sebelumnya

    ScoreCallback onScoreChanged = nullptr;                     // Callback pointer (nullable)
    AsteroidDestroyedCallback onAsteroidDestroyed = nullptr;    // Callback pointer (nullable)

    // Sound variables
    Sound laser;
    Sound error;
    Sound gameover;

public:
    int score = 0;

    GameplayManager() = default; 
    ~GameplayManager(); // DESTRUCTOR UNTUK UNLOAD AUDIO

    // Function overloading: AddScore dengan 1 atau 2 parameter
    void AddScore(int points);                      // Gunakan current combo multiplier
    void AddScore(int basePoints, int multiplier);  // Gunakan multiplier custom

    void SetScoreCallback(ScoreCallback callback);
    void SetAsteroidDestroyedCallback(AsteroidDestroyedCallback callback);

    void textureInit();

    bool isHit();  // Cek collision dengan player

    // LOGIKA GAME

    void update(float deltaTime);   // Update game logic
    void draw();                    // Render game objects
    void reset();                   // Reset ke kondisi awal
};