#include "GameplayManager.h"
#include "AudioManager.h"
#include "raylib.h"
#include "GameConfig.h"
#include "raymath.h"

GameplayManager::~GameplayManager() {
    // no-op: AssetManager handle lifecycle
}

bool GameplayManager::isHit() {
    // Hitbox player: pakai radius shield jika sedang aktif, atau ukuran normal dari config
    int hitRadius = shieldSkill.isActive()
        ? ShieldSkill::SHIELD_RADIUS
        : Config::playerHitbox;

    // Scan semua asteroid aktif yang bertabrakan dengan player
    auto hits = asteroidManager.scanAllAsteroids([hitRadius](const Asteroid& ast) {
        return ast.active && CheckCollisionCircles(
            ast.position, ast.radius, Config::playerStartPos, hitRadius);
    });
    if (!hits.empty()) {
        if (shieldSkill.isActive()) {
            // Shield menyerap damage: hancurkan semua asteroid yang menabrak, lalu konsumsi shield
            shieldSkill.consumeShield();
            AudioManager::getInstance().playSfxOnce("explosion");
            for (auto* ast : hits) {
                explosionManager.spawn(ast->position, 16, ORANGE);
                ast->active = false;
            }
            return false;
        }
        AudioManager::getInstance().playSfxOnce("gameover");
        AudioManager::getInstance().playSfx("explosion");
        explosionManager.spawn(Config::playerStartPos, 35, RED);
        return true;
    }
    return false;
}

void GameplayManager::textureInit() {
    spaceship.init();

    // Hubungkan callback ledakan dari AsteroidManager ke ExplosionManager
    asteroidManager.setExplosionCallback([this](Vector2 pos) {
        explosionManager.spawn(pos, 16, ORANGE);
        AudioManager::getInstance().playSfx("explosion");
    });
}

void GameplayManager::AddScore(int points) {
    // Tambah skor dengan multiplier dari combo stack
    score += points * comboStack.GetMultiplier();
}

void GameplayManager::AddScore(int basePoints, int multiplier) {
    // Overload dengan multiplier kustom, panggil callback jika terdaftar
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
    // Update semua subsistem game
    survivalTime += deltaTime;
    spaceship.update(deltaTime);
    asteroidManager.update(deltaTime);
    shieldSkill.update(deltaTime);
    bombSkill.update(deltaTime);
    explosionManager.update(deltaTime);

    // Cek efek bom shockwave: hancurkan semua asteroid dalam radius lingkaran
    if (bombSkill.isActive()) {
        float r = bombSkill.getShockwaveRadius();
        auto hit = asteroidManager.scanAllAsteroids([r](const Asteroid& a) {
            return a.active && Vector2DistanceSqr(a.position, Config::playerStartPos) <= r * r;
        });
        if (!hit.empty()) {
            AudioManager::getInstance().playSfxOnce("explosion");
            for (auto* ast : hit) {
                explosionManager.spawn(ast->position, 16, ORANGE);
                ast->active = false;
            }
        }
    }

    // Tombol 1: aktifkan shield skill jika sudah siap
    if (IsKeyPressed(KEY_ONE)) {
        shieldSkill.activate();
    }

    // Tombol 2: aktifkan bomb skill jika sudah siap
    if (IsKeyPressed(KEY_TWO)) {
        bombSkill.activate();
    }

    // Tangkap input karakter dari keyboard
    int key = GetCharPressed();
    if (key == 0) return; // Tidak ada input, keluar

    char c = (char)key;
    // Filter: hanya huruf A-Z dan a-z yang diproses
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) return;

    // ===== STATE MACHINE TYPING =====
    if (state == SEARCH_FOR_TARGET) {
        // Cari semua asteroid aktif yang huruf pertamanya cocok dengan input
        auto targets = asteroidManager.scanAllAsteroids([c](const Asteroid& ast) {
            return ast.active && !ast.word.empty() && ast.word[0] == c;
        });
        // Pilih asteroid terdekat dengan player sebagai target
        currentTarget = nullptr;
        for (auto* t : targets) {
            if (currentTarget == nullptr ||
                Vector2DistanceSqr(t->position, Config::playerStartPos) <
                Vector2DistanceSqr(currentTarget->position, Config::playerStartPos)) {
                currentTarget = t;
            }
        }
        if (currentTarget != nullptr) {
            // Kunci target, aktifkan laser, proses karakter pertama
            currentTarget->targeted = true;
            spaceship.activateLaser(currentTarget->position);
            int result = currentTarget->typingAsteroid(c);
            totalKeystrokes++;
            if (result > 0) {
                correctKeystrokes++;
                AudioManager::getInstance().playSfx("laser");
                AddScore(result);
                state = TARGET_LOCKED; // Pindah ke state mengetik
                wasPreviousKeyWrong = false;
            }
        } else {
            // Tidak ada target yang cocok: bunyi error sekali saja per urutan salah
            if (!wasPreviousKeyWrong) {
                AudioManager::getInstance().playSfx("error");
                comboStack.Pop(); // Reset combo karena salah ketik
                wasPreviousKeyWrong = true;
            }
        }
    } else if (state == TARGET_LOCKED) {
        // Jika target sudah tidak aktif (hancur), kembali mencari target baru
        if (currentTarget == nullptr || !currentTarget->active) {
            state = SEARCH_FOR_TARGET;
            currentTarget = nullptr;
            wasPreviousKeyWrong = false;
            return;
        }

        // Proses input karakter berikutnya pada asteroid yang sama
        int result = currentTarget->typingAsteroid(c);
        totalKeystrokes++;
        if (result > 0) {
            correctKeystrokes++;
            AudioManager::getInstance().playSfx("laser");
            AddScore(result);
            wasPreviousKeyWrong = false;

            // Jika seluruh kata telah diketik, asteroid hancur
            if (currentTarget->word.empty()) {
                if (onAsteroidDestroyed != nullptr) onAsteroidDestroyed(currentTarget->originalWord);
                wordsCompleted++;
                // Naikkan level combo setiap 5 kata berhasil diketik
                if (wordsCompleted >= 5) {
                    comboStack.Push();
                    wordsCompleted = 0;
                }
                state = SEARCH_FOR_TARGET;
                currentTarget = nullptr;
            } else {
                // Update posisi laser mengikuti target yang bergerak
                spaceship.activateLaser(currentTarget->position);
            }
        } else {
            // Salah ketik di tengah-tengah kata: error sound sekali saja
            if (!wasPreviousKeyWrong) {
                AudioManager::getInstance().playSfx("error");
                comboStack.Pop();
                wasPreviousKeyWrong = true;
            }
        }
    }
}

void GameplayManager::draw() {
    spaceship.draw();
    asteroidManager.draw();
    explosionManager.draw();
    shieldSkill.draw();
    bombSkill.draw();
    int multiplier = comboStack.GetMultiplier();

    // Tampilkan skor di tengah atas layar
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    DrawText(scoreText, Config::screenWidth/2 - MeasureText(scoreText, 20)/2, 30, 20, WHITE);

    // Tampilkan multiplier combo (kuning jika >1, abu-abu jika 1)
    const char* comboText = TextFormat("%dx COMBO", multiplier);
    DrawText(comboText, Config::screenWidth/2 - MeasureText(comboText, 30)/2, 55, 30, (multiplier > 1) ? YELLOW : GRAY);

    // Progress bar combo: 5 words per level, maksimum 64 sudah penuh
    if (multiplier != 64) {
        char progressText[50];
        snprintf(progressText, sizeof(progressText), "%d/5 words", wordsCompleted);
        DrawText(progressText, Config::screenWidth/2 - MeasureText(progressText, 20)/2, 85, 20, (multiplier > 1) ? GREEN : DARKGRAY);
    }

    // HUD status Shield (pojok kanan atas)
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

    // HUD status Bomb (pojok kanan atas, di bawah shield)
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
    // Reset semua state ke kondisi awal permainan
    score = 0;
    totalKeystrokes = 0;
    correctKeystrokes = 0;
    survivalTime = 0.0f;
    state = typingState::SEARCH_FOR_TARGET;
    currentTarget = nullptr;
    spaceship = SpaceShip();
    comboStack.Reset();
    wordsCompleted = 0;
    wasPreviousKeyWrong = false;
    explosionManager.reset();
}