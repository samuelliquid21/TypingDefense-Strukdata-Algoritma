#include "GameplayManager.h"
#include "raylib.h"
#include "GameConfig.h"
#include "raymath.h"

GameplayManager::~GameplayManager() {
    UnloadSound(laser);
    UnloadSound(error);
    UnloadSound(gameover);
}

bool GameplayManager::isHit() {
    int hitRadius = shieldSkill.isActive()
        ? ShieldSkill::SHIELD_RADIUS
        : Config::playerHitbox;

    auto hits = asteroidManager.scanAllAsteroids([hitRadius](const Asteroid& ast) {
        return ast.active && CheckCollisionCircles(
            ast.position, ast.radius, Config::playerStartPos, hitRadius);
    });
    if (!hits.empty()) {
        if (shieldSkill.isActive()) {
            shieldSkill.consumeShield();
            for (auto* ast : hits) ast->active = false;
            return false;
        }
        if (!IsSoundPlaying(gameover)) PlaySound(gameover);
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
    shieldSkill.update(deltaTime);
    bombSkill.update(deltaTime);

    if (bombSkill.isActive()) {
        float r = bombSkill.getShockwaveRadius();
        auto hit = asteroidManager.scanAllAsteroids([r](const Asteroid& a) {
            return a.active && Vector2DistanceSqr(a.position, Config::playerStartPos) <= r * r;
        });
        for (auto* ast : hit) ast->active = false;
    }

    if (IsKeyPressed(KEY_ONE)) {
        shieldSkill.activate();
    }

    if (IsKeyPressed(KEY_TWO)) {
        bombSkill.activate();
    }

    int key = GetCharPressed();
    if (key == 0) return; // Keluar kalau tidak ada tombol ditekan

    char c = (char)key;
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) return; // hanya huruf

    if (state == SEARCH_FOR_TARGET) {
        auto targets = asteroidManager.scanAllAsteroids([c](const Asteroid& ast) {
            return ast.active && !ast.word.empty() && ast.word[0] == c;
        });
        currentTarget = nullptr;
        for (auto* t : targets) {
            if (currentTarget == nullptr ||
                Vector2DistanceSqr(t->position, Config::playerStartPos) <
                Vector2DistanceSqr(currentTarget->position, Config::playerStartPos)) {
                currentTarget = t;
            }
        }
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
                if (onAsteroidDestroyed != nullptr) onAsteroidDestroyed(currentTarget->originalWord);
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
    shieldSkill.draw();
    bombSkill.draw();
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

    // Shield status HUD
    const char* shieldText;
    Color shieldColor;
    if (shieldSkill.isReady()) {
        shieldText = "SHIELD: READY [1]";
        shieldColor = GREEN;
    } else if (shieldSkill.isActive()) {
        shieldText = "SHIELD: ACTIVE";
        shieldColor = SKYBLUE;
    } else {
        float remaining = Config::shieldCooldown * (1.0f - shieldSkill.getCooldownProgress());
        shieldText = TextFormat("SHIELD: %.0fs", remaining);
        shieldColor = GRAY;
    }
    DrawText(shieldText, Config::screenWidth - MeasureText(shieldText, 15) - 10, 10, 15, shieldColor);

    // Bomb status HUD
    const char* bombText;
    Color bombColor;
    if (bombSkill.isReady()) {
        bombText = "BOMB: READY [2]";
        bombColor = ORANGE;
    } else if (bombSkill.isActive()) {
        bombText = "BOMB: ACTIVE";
        bombColor = RED;
    } else {
        float remaining = Config::bombCooldown * (1.0f - bombSkill.getCooldownProgress());
        bombText = TextFormat("BOMB: %.0fs", remaining);
        bombColor = GRAY;
    }
    DrawText(bombText, Config::screenWidth - MeasureText(bombText, 15) - 10, 28, 15, bombColor);
}

void GameplayManager::reset() {
    score = 0;
    state = typingState::SEARCH_FOR_TARGET;
    currentTarget = nullptr;
    spaceship = SpaceShip();
    comboStack.Reset();
    wordsCompleted = 0;
    wasPreviousKeyWrong = false;
}