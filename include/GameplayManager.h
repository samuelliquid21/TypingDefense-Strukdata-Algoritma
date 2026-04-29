#pragma once

#include "SpaceShip.h"
#include "AsteroidManager.h"
#include "Combostack.h"
#include "raylib.h"

enum typingState {
    SEARCH_FOR_TARGET,
    TARGET_LOCKED
};

using ScoreCallback = void(*)(int score, int multiplier);
using AsteroidDestroyedCallback = void(*)(const char* word);

class GameplayManager
{
private:
    SpaceShip spaceship;
    AsteroidManager asteroidManager;
    ComboStack comboStack;
    typingState state = typingState::SEARCH_FOR_TARGET;
    Asteroid* currentTarget = nullptr;
    int wordsCompleted = 0;
    bool wasPreviousKeyWrong = false;

    ScoreCallback onScoreChanged = nullptr;
    AsteroidDestroyedCallback onAsteroidDestroyed = nullptr;

    // Nama variabel yang lu buat tadi
    Sound laser;
    Sound error;
    Sound gameover;

public:
    int score = 0;

    GameplayManager() = default; 
    ~GameplayManager(); // DESTRUCTOR UNTUK UNLOAD AUDIO

    void AddScore(int points);
    void AddScore(int basePoints, int multiplier);

    void SetScoreCallback(ScoreCallback callback);
    void SetAsteroidDestroyedCallback(AsteroidDestroyedCallback callback);

    void textureInit();
    bool isHit();
    void update(float deltaTime);
    void draw();
    void reset();
};