#include "GameplayManager.h"
#include "raylib.h"
#include "GameConfig.h"

bool GameplayManager::isHit() {
    return asteroidManager.collisionWithPlayer();
}

void GameplayManager::textureInit() {
    spaceship.init();
}

// Function overloading: AddScore dengan 1 parameter (gunakan current combo)
void GameplayManager::AddScore(int points) {
    score += points * comboStack.GetMultiplier();
}

// Function overloading: AddScore dengan 2 parameter (custom multiplier)
// Juga memanggil callback jika ada
void GameplayManager::AddScore(int basePoints, int multiplier) {
    score += basePoints * multiplier;

    // Execute callback function jika terdaftar
    if (onScoreChanged != nullptr) {
        onScoreChanged(score, multiplier);
    }
}

// Setter untuk score callback function
void GameplayManager::SetScoreCallback(ScoreCallback callback) {
    onScoreChanged = callback;
}

// Setter untuk asteroid destroyed callback function
void GameplayManager::SetAsteroidDestroyedCallback(AsteroidDestroyedCallback callback) {
    onAsteroidDestroyed = callback;
}

void GameplayManager::update(float deltaTime) {
    spaceship.update(deltaTime);
    asteroidManager.update(deltaTime);

    char c = GetCharPressed();
    if (c == '\0') {
        wasPreviousKeyWrong = false;
        return;
    }

    if (state == SEARCH_FOR_TARGET) {
        currentTarget = asteroidManager.getTarget(c);
        if (currentTarget != nullptr) {
            currentTarget->targeted = true;
            spaceship.activateLaser(currentTarget->position);
            int result = currentTarget->typingAsteroid(c);
            if (result > 0) {
                AddScore(result);
                state = TARGET_LOCKED;
                wasPreviousKeyWrong = false;
            } else {
                if (wasPreviousKeyWrong == false) {
                    comboStack.Pop();
                    wasPreviousKeyWrong = true;
                }
            }
        } else {
            if (wasPreviousKeyWrong == false) {
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
        
        bool hadContentBefore = !currentTarget->word.empty();
        int result = currentTarget->typingAsteroid(c);
        
        if (result > 0) {
            AddScore(result);
            wasPreviousKeyWrong = false;
            
            if (currentTarget->word.empty()) {
                // Execute callback function saat asteroid hancur
                if (onAsteroidDestroyed != nullptr) {
                    onAsteroidDestroyed(currentTarget->word.c_str());
                }
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
            if (wasPreviousKeyWrong == false && hadContentBefore) {
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
    
    // Score (center top - above combo)
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    int scoreWidth = MeasureText(scoreText, 20);
    DrawText(scoreText, Config::screenWidth/2 - scoreWidth/2, 30, 20, WHITE);
    
    // Combo (center top - below score)
    const char* comboText = TextFormat("%dx COMBO", multiplier);
    int comboWidth = MeasureText(comboText, 30);
    Color comboColor = (multiplier > 1) ? YELLOW : GRAY;
    DrawText(comboText, Config::screenWidth/2 - comboWidth/2, 55, 30, comboColor);
    
    // Progress (center below combo - hidden at 64x)
    if (multiplier != 64) {
        char progressText[50];
        snprintf(progressText, sizeof(progressText), "%d/5 words", wordsCompleted);
        int progressWidth = MeasureText(progressText, 20);
        Color progressColor = (multiplier > 1) ? GREEN : DARKGRAY;
        DrawText(progressText, Config::screenWidth/2 - progressWidth/2, 85, 20, progressColor);
    }
    
    // Pause hint (bottom right)
    DrawText("[ESC] For Pause", Config::screenWidth - 180, Config::screenHeight - 40, 20, LIGHTGRAY);
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