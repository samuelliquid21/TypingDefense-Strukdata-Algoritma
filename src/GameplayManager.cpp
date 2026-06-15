#include "GameplayManager.h"
#include "SkinManager.h"
#include "raylib.h"
#include "GameConfig.h"
#include "raymath.h"

GameplayManager::~GameplayManager() {
    UnloadSound(laser);
    UnloadSound(error);
    UnloadSound(gameover);
    UnloadSound(sfxShimmer);
    UnloadSound(sfxGlitch);
    if (sfxWind.stream.buffer != nullptr) {
        StopSound(sfxWind);
        UnloadSound(sfxWind);
    }
}

bool GameplayManager::isHit() {
    if (isExploding || isShipDead) return false;

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
            if (sfxGlitch.stream.buffer != nullptr) {
                SetSoundVolume(sfxGlitch, 0.35f);
                PlaySound(sfxGlitch);
            }
            return false;
        }
        isExploding = true;
        explosionTimer = 1.2f;
        explosion.Trigger(Config::playerStartPos);
        wordsAtExplosionStart = wordsCompleted;
        if (sfxGlitch.stream.buffer != nullptr) {
            SetSoundVolume(sfxGlitch, 0.5f);
            PlaySound(sfxGlitch);
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

    // Load SFX from assets/sfx/
    sfxShimmer = {0};
    sfxGlitch = {0};
    sfxWind = {0};
    windPlaying = false;

    if (FileExists("assets/sfx/shimmering.mp3")) {
        sfxShimmer = LoadSound("assets/sfx/shimmering.mp3");
        SetSoundVolume(sfxShimmer, 0.25f);
    }
    if (FileExists("assets/sfx/sounded_glitch.mp3")) {
        sfxGlitch = LoadSound("assets/sfx/sounded_glitch.mp3");
        SetSoundVolume(sfxGlitch, 0.3f);
    }
    if (FileExists("assets/sfx/wind_milky.mp3")) {
        sfxWind = LoadSound("assets/sfx/wind_milky.mp3");
        SetSoundVolume(sfxWind, 0.12f);
    }

    isExploding = false;
    isShipDead = false;
    explosionTimer = 0.0f;
    wordsAtExplosionStart = 0;
    sessionRP = 0;
}

void GameplayManager::setSkinFromManager() {
    int skinId = SkinManager::getInstance().getActiveSkin();
    spaceship.setSkin(skinId);
}

void GameplayManager::AddScore(int points) {
    int mult = comboStack.GetMultiplier();
    score += points * mult;

    if (mult > 1 && sfxShimmer.stream.buffer != nullptr) {
        SetSoundVolume(sfxShimmer, 0.25f);
        PlaySound(sfxShimmer);
    }
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
    // Handle explosion state
    if (isExploding) {
        explosionTimer -= deltaTime;
        explosion.Update(deltaTime);
        if (explosionTimer <= 0.0f) {
            isExploding = false;
            isShipDead = true;
            if (sfxGlitch.stream.buffer != nullptr) StopSound(sfxGlitch);
            if (gameover.stream.buffer != nullptr && !IsSoundPlaying(gameover)) {
                PlaySound(gameover);
            }
        }
        return;
    }

    if (isShipDead) return;

    spaceship.update(deltaTime);
    asteroidManager.update(deltaTime);
    shieldSkill.update(deltaTime);
    bombSkill.update(deltaTime);

    if (sfxWind.stream.buffer != nullptr && !IsSoundPlaying(sfxWind)) {
        PlaySound(sfxWind);
    }

    if (IsKeyPressed(KEY_TWO) && bombSkill.isReady() && sfxGlitch.stream.buffer != nullptr) {
        SetSoundVolume(sfxGlitch, 0.3f);
        PlaySound(sfxGlitch);
    }

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
        sessionRP += 15;
    }

    int key = GetCharPressed();
    if (key == 0) return;

    char c = (char)key;
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) return;

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
                wasPreviousKeyWrong = false;
            }
        } else {
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
            wasPreviousKeyWrong = false;

            if (currentTarget->word.empty()) {
                if (onAsteroidDestroyed != nullptr) onAsteroidDestroyed("");
                sessionRP += 3;
                wordsCompleted++;
                if (wordsCompleted >= 5) {
                    comboStack.Push();
                    sessionRP += 5;
                    wordsCompleted = 0;
                }
                state = SEARCH_FOR_TARGET;
                currentTarget = nullptr;
            } else {
                spaceship.activateLaser(currentTarget->position);
            }
        } else {
            if (!wasPreviousKeyWrong) {
                PlaySound(error);
                comboStack.Pop();
                wasPreviousKeyWrong = true;
            }
        }
    }
}

void GameplayManager::draw() {
    if (isExploding) {
        explosion.Draw();
        return;
    }

    spaceship.draw();
    asteroidManager.draw();
    shieldSkill.draw();
    bombSkill.draw();
    int multiplier = comboStack.GetMultiplier();

    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    DrawText(scoreText, Config::screenWidth/2 - MeasureText(scoreText, 20)/2, 30, 20, WHITE);

    char rpText[50];
    int totalRP = SkinManager::getInstance().getRP();
    snprintf(rpText, sizeof(rpText), "RP: %d (+%d)", totalRP, sessionRP);
    DrawText(rpText, Config::screenWidth - MeasureText(rpText, 16) - 10, 50, 16, GOLD);

    const char* comboText = TextFormat("%dx COMBO", multiplier);
    DrawText(comboText, Config::screenWidth/2 - MeasureText(comboText, 30)/2, 55, 30, (multiplier > 1) ? YELLOW : GRAY);

    if (multiplier != 64) {
        char progressText[50];
        snprintf(progressText, sizeof(progressText), "%d/5 words", wordsCompleted);
        DrawText(progressText, Config::screenWidth/2 - MeasureText(progressText, 20)/2, 85, 20, (multiplier > 1) ? GREEN : DARKGRAY);
    }

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

    // Show warning during explosion
    if (isExploding) {
        const char* warn = "SHIP DESTROYED";
        int warnW = MeasureText(warn, 40);
        float pulse = 0.6f + 0.4f * sinf(GetTime() * 8.0f);
        DrawText(warn, Config::screenWidth/2 - warnW/2, Config::screenHeight/2 - 80, 40,
            { 255, 50, 50, (unsigned char)(pulse * 255) });
    }
}

void GameplayManager::reset() {
    score = 0;
    state = typingState::SEARCH_FOR_TARGET;
    currentTarget = nullptr;
    spaceship = SpaceShip();
    comboStack.Reset();
    wordsCompleted = 0;
    wasPreviousKeyWrong = false;
    isExploding = false;
    isShipDead = false;
    explosionTimer = 0.0f;
}