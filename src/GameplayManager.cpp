#include "GameplayManager.h"
#include "AudioManager.h"
#include "raylib.h"
#include "GameConfig.h"
#include "raymath.h"

// ===============================
// 🎮 MANAGER GAMEPLAY
// ===============================
GameplayManager::~GameplayManager() {
    // no-op: AssetManager handle lifecycle
}

bool GameplayManager::isHit() {
    // Hitbox player: pakai radius aura, shield, atau ukuran normal
    int hitRadius = getHitRadius();

    // Scan semua asteroid aktif yang bertabrakan dengan player
    auto hits = asteroidManager.scanAllAsteroids([hitRadius](const Asteroid& ast) {
        return ast.active && CheckCollisionCircles(
            ast.position, ast.radius, Config::playerStartPos, hitRadius);
    });
    if (hits.empty()) return false;
    if (handleAuraAbsorption(hits)) return false;
    if (handleShieldAbsorption(hits)) return false;
    triggerGameOver();
    return true;
}

// Hitung radius deteksi tabrakan berdasarkan skill aktif
int GameplayManager::getHitRadius() const {
    if (auraFieldSkill.isActive()) return AuraFieldSkill::AURA_RADIUS;
    if (shieldSkill.isActive()) return ShieldSkill::SHIELD_RADIUS;
    return Config::playerHitbox;
}

// Tangani tabrakan saat aura aktif: hancurkan asteroid tanpa damage ke player
bool GameplayManager::handleAuraAbsorption(const std::vector<Asteroid*>& hits) {
    if (!auraFieldSkill.isActive()) return false;
    // Aura menyerap semua damage, tetap aktif sampai durasi habis
    AudioManager::getInstance().playSfxOnce("explosion");
    for (auto* ast : hits) {
        explosionManager.spawn(ast->position, 16, ORANGE);
        ast->active = false;
    }
    return true;
}

// Tangani tabrakan saat shield aktif: hancurkan asteroid dan konsumsi shield
bool GameplayManager::handleShieldAbsorption(const std::vector<Asteroid*>& hits) {
    if (!shieldSkill.isActive()) return false;
    // Shield menyerap damage: hancurkan semua asteroid yang menabrak, lalu konsumsi shield
    shieldSkill.consumeShield();
    AudioManager::getInstance().playSfxOnce("explosion");
    for (auto* ast : hits) {
        explosionManager.spawn(ast->position, 16, ORANGE);
        ast->active = false;
    }
    return true;
}

// Pemain terkena damage: mainkan efek game over dan ledakan besar
void GameplayManager::triggerGameOver() {
    AudioManager::getInstance().playSfxOnce("gameover");
    AudioManager::getInstance().playSfx("explosion");
    explosionManager.spawn(Config::playerStartPos, 35, RED);
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
    int mult = scoreBoosterSkill.isActive() ? ScoreBoosterSkill::SCORE_MULTIPLIER : 1;
    score += points * comboStack.GetMultiplier() * mult;
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

void GameplayManager::setUnlockedSkills(const std::vector<std::string>* skills) {
    m_unlockedSkills = skills;
    rebuildActiveSkills();
}

bool GameplayManager::isUnlocked(const std::string& name) const {
    if (!m_unlockedSkills) return false;
    for (const auto& s : *m_unlockedSkills) {
        if (s == name) return true;
    }
    return false;
}

void GameplayManager::rebuildActiveSkills() {
    m_activeSkills.clear();
    int key = 1;

    struct { Skill* skill; const char* keyName; } all[] = {
        {&shieldSkill,       "barrier"},
        {&auraFieldSkill,    "aura_field"},
        {&bombSkill,         "shockwave"},
        {&instantCritSkill,  "instant_crit"},
        {&scoreBoosterSkill, "score_booster"},
    };

    for (auto& entry : all) {
        if (isUnlocked(entry.keyName)) {
            m_activeSkills.push_back({entry.skill, entry.keyName, key++});
        }
    }
}

void GameplayManager::update(float deltaTime) {
    // Update semua subsistem game
    updateSubsystems(deltaTime);

    // Update & keypress untuk skill aktif yang ter-unlock
    updateActiveSkills(deltaTime);

    // Cek efek bom shockwave: hancurkan semua asteroid dalam radius lingkaran
    handleBombShockwave();

    // ===== STATE MACHINE TYPING =====
    // Loop semua karakter input yang di-buffer agar typing lebih responsif
    processAllCharInputs();
}

// Update subsistem game: survival time, spaceship, asteroid, dan efek ledakan
void GameplayManager::updateSubsystems(float deltaTime) {
    survivalTime += deltaTime;
    spaceship.update(deltaTime);
    asteroidManager.update(deltaTime);
    explosionManager.update(deltaTime);
}

// Update keypress dan cooldown untuk semua skill aktif
void GameplayManager::updateActiveSkills(float deltaTime) {
    for (auto& binding : m_activeSkills) {
        if (IsKeyPressed(KEY_ZERO + binding.keyNumber)) {
            binding.skill->activate();
        }
        binding.skill->update(deltaTime);
    }
}

// Proses efek bom shockwave: hancurkan semua asteroid dalam radius lingkaran
void GameplayManager::handleBombShockwave() {
    if (!bombSkill.isActive()) return;
    float r = bombSkill.getShockwaveRadius();
    auto hit = asteroidManager.scanAllAsteroids([r](const Asteroid& a) {
        return a.active && Vector2DistanceSqr(a.position, Config::playerStartPos) <= r * r;
    });
    if (!hit.empty()) {
        AudioManager::getInstance().playSfxOnce("explosion");
        for (auto* ast : hit) {
            explosionManager.spawn(ast->position, 16, ORANGE);
            ast->active = false;
            AddScore((int)ast->originalWord.length());
        }
    }
}

// Loop semua karakter input yang di-buffer dan proses sesuai state typing
void GameplayManager::processAllCharInputs() {
    int key = GetCharPressed();
    while (key != 0) {
        char c = (char)key;
        // Filter: hanya huruf A-Z dan a-z yang diproses
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            if (state == SEARCH_FOR_TARGET) {
                processSearchForTarget(c);
            } else if (state == TARGET_LOCKED) {
                if (isTargetInvalid()) {
                    resetToSearchTarget();
                    key = GetCharPressed();
                    continue;
                }
                processTargetTyping(c);
            }
        }
        key = GetCharPressed();
    }
}

// Cari asteroid terdekat yang huruf pertamanya cocok dengan input
Asteroid* GameplayManager::findNearestTarget(char c) {
    auto targets = asteroidManager.scanAllAsteroids([c](const Asteroid& ast) {
        return ast.active && !ast.word.empty() && ast.word[0] == c;
    });
    Asteroid* nearest = nullptr;
    for (auto* t : targets) {
        if (nearest == nullptr ||
            Vector2DistanceSqr(t->position, Config::playerStartPos) <
            Vector2DistanceSqr(nearest->position, Config::playerStartPos)) {
            nearest = t;
        }
    }
    return nearest;
}

// Tangani error saat tidak ada target yang cocok dengan input
void GameplayManager::playSearchError() {
    if (!wasPreviousKeyWrong) {
        AudioManager::getInstance().playSfx("error");
        comboStack.Pop();
        wasPreviousKeyWrong = true;
    }
}

// Proses state SEARCH_FOR_TARGET: cari target, kunci, dan ketik karakter pertama
void GameplayManager::processSearchForTarget(char c) {
    currentTarget = findNearestTarget(c);
    if (currentTarget == nullptr) {
        playSearchError();
        return;
    }
    // Kunci target, aktifkan laser, proses karakter pertama
    currentTarget->targeted = true;
    spaceship.activateLaser(currentTarget->position);
    int result = currentTarget->typingAsteroid(c);
    totalKeystrokes++;
    if (result > 0) {
        correctKeystrokes++;
        AudioManager::getInstance().playSfx("laser");
        AddScore(result);
        handleInstantCritOrLock();
    }
}

// Tangani hasil setelah karakter pertama berhasil: instant crit atau lock target
void GameplayManager::handleInstantCritOrLock() {
    if (instantCritSkill.isActive() && currentTarget->active) {
        currentTarget->word = "";
        currentTarget->active = false;
        if (currentTarget->onDestroyed) currentTarget->onDestroyed(currentTarget->position);
        if (onAsteroidDestroyed != nullptr) onAsteroidDestroyed(currentTarget->originalWord);
        wordsCompleted++;
        if (wordsCompleted >= 5) {
            comboStack.Push();
            wordsCompleted = 0;
        }
        state = SEARCH_FOR_TARGET;
        currentTarget = nullptr;
        wasPreviousKeyWrong = false;
    } else {
        state = TARGET_LOCKED;
        wasPreviousKeyWrong = false;
    }
}

// Proses state TARGET_LOCKED: ketik karakter berikutnya pada asteroid yang sama
void GameplayManager::processTargetTyping(char c) {
    int result = currentTarget->typingAsteroid(c);
    totalKeystrokes++;
    if (result > 0) {
        handleSuccessfulTyping(result);
    } else {
        if (!wasPreviousKeyWrong) {
            AudioManager::getInstance().playSfx("error");
            comboStack.Pop();
            wasPreviousKeyWrong = true;
        }
    }
}

// Tangani input karakter yang benar: update skor dan cek apakah kata selesai
void GameplayManager::handleSuccessfulTyping(int result) {
    correctKeystrokes++;
    AudioManager::getInstance().playSfx("laser");
    AddScore(result);
    wasPreviousKeyWrong = false;

    // Jika seluruh kata telah diketik, asteroid hancur
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
}

// Cek apakah target saat ini tidak valid (null, tidak aktif, atau tidak ditarget)
bool GameplayManager::isTargetInvalid() const {
    return currentTarget == nullptr || !currentTarget->active || !currentTarget->targeted;
}

// Reset state typing kembali ke SEARCH_FOR_TARGET
void GameplayManager::resetToSearchTarget() {
    state = SEARCH_FOR_TARGET;
    currentTarget = nullptr;
    wasPreviousKeyWrong = false;
}

void GameplayManager::draw() {
    drawGameObjects();
    drawScoreAndCombo();
    drawActiveSkillsHUD();
}

// Render semua objek game: spaceship, asteroid, ledakan, dan efek skill
void GameplayManager::drawGameObjects() {
    spaceship.draw();
    asteroidManager.draw();
    explosionManager.draw();

    // Draw visual untuk skill yang ter-unlock
    for (auto& binding : m_activeSkills) {
        binding.skill->draw();
    }
}

// Tampilkan skor, multiplier combo, dan progress bar combo di tengah atas
void GameplayManager::drawScoreAndCombo() {
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
}

// Tampilkan HUD skill aktif di pojok kanan atas
void GameplayManager::drawActiveSkillsHUD() {
    int hudY = 10;
    for (auto& binding : m_activeSkills) {
        drawSkillBindingLine(binding, hudY);
    }
}

// Gambar satu baris HUD untuk satu binding skill
void GameplayManager::drawSkillBindingLine(const SkillBinding& binding, int& hudY) {
    const char* keyStr = TextFormat("[%d]", binding.keyNumber);
    const char* name = binding.skill->getName();
    Color color;
    char line[64];

    if (binding.skill->isReady()) {
        snprintf(line, sizeof(line), "%s: READY %s", name, keyStr);
        color = GREEN;
    } else if (binding.skill->isActive()) {
        snprintf(line, sizeof(line), "%s: ACTIVE", name);
        color = SKYBLUE;
    } else {
        // Gunakan cooldown skill — ambil dari getName untuk lookup (tidak 100% akurat)
        // Alternatif: simpan cooldown value di SkillBinding
        float remaining = 30.0f * (1.0f - binding.skill->getCooldownProgress());
        snprintf(line, sizeof(line), "%s: %.0fs", name, remaining);
        color = GRAY;
    }
    DrawText(line, Config::screenWidth - MeasureText(line, 15) - 10, hudY, 15, color);
    hudY += 18;
}

