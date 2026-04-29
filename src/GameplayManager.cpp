#include "GameplayManager.h"
#include "raylib.h"
#include "GameConfig.h"

GameplayManager::~GameplayManager() {
    UnloadSound(laser);
    UnloadSound(error);
    UnloadSound(gameover);
}

bool GameplayManager::isHit() {
    if (asteroidManager.collisionWithPlayer()) {
        // Cek supaya suara gameover tidak dipicu berulang kali dalam satu frame
        if (!IsSoundPlaying(gameover)) { 
            PlaySound(gameover);
        }
        return true;
    }
    return false;
}

void GameplayManager::textureInit() {
    spaceship.init();
    laser = LoadSound("assets/sound/laser.mp3");
    error = LoadSound("assets/sound/error.mp3");
    gameover = LoadSound("assets/sound/gameover.mp3");
    
    SetSoundVolume(laser, 0.4f);
    SetSoundVolume(error, 0.5f);
    SetSoundVolume(gameover, 0.8f);
}

void GameplayManager::AddScore(int points) {
    score += points * comboStack.GetMultiplier();
}

void GameplayManager::AddScore(int basePoints, int multiplier) {
    score += basePoints * multiplier;
    if (onScoreChanged != nullptr) {
        onScoreChanged(score, multiplier);
    }
}

void GameplayManager::SetScoreCallback(ScoreCallback callback) {
    onScoreChanged = callback;
}

void GameplayManager::SetAsteroidDestroyedCallback(AsteroidDestroyedCallback callback) {
    onAsteroidDestroyed = callback;
}

void GameplayManager::update(float deltaTime) {
    spaceship.update(deltaTime);
    asteroidManager.update(deltaTime);

    int key = GetCharPressed();
    if (key == 0) return; // Keluar kalau tidak ada tombol ditekan

    char c = (char)key;

    if (state == SEARCH_FOR_TARGET) {
        currentTarget = asteroidManager.getTarget(c);
        if (currentTarget != nullptr) {
            currentTarget->targeted = true;
            spaceship.activateLaser(currentTarget->position);
            int result = currentTarget->typingAsteroid(c);
            if (result > 0) {
                PlaySound(laser);
                AddScore(result);
                state = TARGET_LOCKED;
                wasPreviousKeyWrong = false; // Reset status salah
            }
        } else {
            // Bunyi error hanya jika penekanan sebelumnya BENAR
            if (!wasPreviousKeyWrong) {
                PlaySound(error);
                comboStack.Pop();
                wasPreviousKeyWrong = true;
            }
        }
    } else if (state == TARGET_LOCKED) {
        if (currentTarget == nullptr || !currentTarget->active) {
            state = SEARCH_FOR_TARGET;
            currentTarget = nullptr;
            wasPreviousKeyWrong = false;
            return;
        }
        
        int result = currentTarget->typingAsteroid(c);
        if (result > 0) {
            PlaySound(laser);
            AddScore(result);
            wasPreviousKeyWrong = false; // Reset status salah karena sudah benar
            
            if (currentTarget->word.empty()) {
                if (onAsteroidDestroyed != nullptr) onAsteroidDestroyed("");
                wordsCompleted++;
                if (wordsCompleted >= 5) {
                    comboStack.Push();
                    wordsCompleted = 0;
                }
                state = SEARCH_FOR_TARGET;
                currentTarget = nullptr;
            } else {
                spaceship.activateLaser(currentTarget->position);
            }
        } else {
            // Bunyi error jika salah ketik di tengah kata
            if (!wasPreviousKeyWrong) {
                PlaySound(error);
                comboStack.Pop();
                wasPreviousKeyWrong = true;
            }
        }
    }
}

void GameplayManager::draw() {
    spaceship.draw();
    asteroidManager.draw();
    int multiplier = comboStack.GetMultiplier();
    
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    DrawText(scoreText, Config::screenWidth/2 - MeasureText(scoreText, 20)/2, 30, 20, WHITE);
    
    const char* comboText = TextFormat("%dx COMBO", multiplier);
    DrawText(comboText, Config::screenWidth/2 - MeasureText(comboText, 30)/2, 55, 30, (multiplier > 1) ? YELLOW : GRAY);
    
    if (multiplier != 64) {
        char progressText[50];
        snprintf(progressText, sizeof(progressText), "%d/5 words", wordsCompleted);
        DrawText(progressText, Config::screenWidth/2 - MeasureText(progressText, 20)/2, 85, 20, (multiplier > 1) ? GREEN : DARKGRAY);
    }
}

void GameplayManager::reset() {
    score = 0;
    state = typingState::SEARCH_FOR_TARGET;
    currentTarget = nullptr;
    spaceship = SpaceShip();
    asteroidManager.clear();
    comboStack.Reset();
    wordsCompleted = 0;
    wasPreviousKeyWrong = false;
}