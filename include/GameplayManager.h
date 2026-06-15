#pragma once

#include "SpaceShip.h"
#include "AsteroidManager.h"
#include "Combostack.h"
#include "ShieldSkill.h"
#include "BombSkill.h"
#include "ExplosionEffect.h"
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
    ShieldSkill shieldSkill;
    BombSkill bombSkill;
    typingState state = typingState::SEARCH_FOR_TARGET;
    Asteroid* currentTarget = nullptr;
    int wordsCompleted = 0;
    bool wasPreviousKeyWrong = false;

    ScoreCallback onScoreChanged = nullptr;
    AsteroidDestroyedCallback onAsteroidDestroyed = nullptr;

    Sound laser;
    Sound error;
    Sound gameover;

    // SFX from assets/sfx/
    Sound sfxShimmer;
    Sound sfxGlitch;
    Sound sfxWind;
    bool windPlaying;

    // Explosion
    ExplosionEffect explosion;
    bool isExploding;
    bool isShipDead;
    float explosionTimer;
    int wordsAtExplosionStart;

public:
    int score = 0;
    int sessionRP = 0;

    GameplayManager() = default;
    ~GameplayManager();

    void AddScore(int points);
    void AddScore(int basePoints, int multiplier);

    void SetScoreCallback(ScoreCallback callback);
    void SetAsteroidDestroyedCallback(AsteroidDestroyedCallback callback);

    void textureInit();
    void setSkinFromManager();

    bool isHit();
    bool isShipDestroyed() const { return isShipDead; }
    bool isExplodingNow() const { return isExploding; }
    int getSessionRP() const { return sessionRP; }

    void update(float deltaTime);
    void draw();
    void reset();
};