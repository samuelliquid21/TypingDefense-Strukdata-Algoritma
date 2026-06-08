#include "ShieldSkill.h"
#include "raylib.h"
#include "GameConfig.h"

ShieldSkill::ShieldSkill() {}

// Aktifkan shield: hanya bisa dari state IDLE
void ShieldSkill::activate() {
    if (state != IDLE) return; // Abaikan jika sedang aktif atau cooldown
    state = ACTIVE;
    timer = 0.0f;
}

// Update cooldown timer setiap frame
void ShieldSkill::update(float deltaTime) {
    if (state == COOLDOWN) {
        timer += deltaTime;
        // Cooldown selesai → kembali IDLE
        if (timer >= Config::shieldCooldown) {
            state = IDLE;
            timer = 0.0f;
        }
    }
    // IDLE dan ACTIVE tidak perlu update timer
}

// Render visual shield di sekitar posisi player
void ShieldSkill::draw() {
    if (state == IDLE) {
        // Lingkaran tipis transparan — indikasi shield siap dipakai
        DrawCircleLines(
            Config::playerStartPos.x,
            Config::playerStartPos.y,
            48,
            {0, 200, 255, 30}
        );
    } else if (state == ACTIVE) {
        // Lingkaran ganda terang — shield aktif melindungi
        DrawCircleLines(
            Config::playerStartPos.x,
            Config::playerStartPos.y,
            48,
            {0, 200, 255, 100}
        );
        DrawCircleLines(
            Config::playerStartPos.x,
            Config::playerStartPos.y,
            52,
            {0, 200, 255, 50}
        );
    }
    // COOLDOWN: tidak ada visual (shield habis)
}

const char* ShieldSkill::getName() const {
    return "Shield";
}

bool ShieldSkill::isReady() const {
    return state == IDLE;
}

bool ShieldSkill::isActive() const {
    return state == ACTIVE;
}

// Progress cooldown: 0.0 saat IDLE/ACTIVE, 0.0–1.0 saat COOLDOWN
float ShieldSkill::getCooldownProgress() const {
    if (state == IDLE || state == ACTIVE) return 0.0f;
    return timer / Config::shieldCooldown;
}

// Konsumsi shield saat terkena hit: ACTIVE → COOLDOWN, return true jika menyerap
bool ShieldSkill::consumeShield() {
    if (state != ACTIVE) return false; // Shield tidak aktif → tidak menyerap
    state = COOLDOWN;
    timer = 0.0f;
    return true;
}
