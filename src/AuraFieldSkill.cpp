#include "AuraFieldSkill.h"
#include "raylib.h"
#include "GameConfig.h"

// ===============================
// 🛡️ AURA FIELD SKILL
// ===============================

namespace {
    // Lingkaran tipis transparan — indikasi aura siap dipakai
    void drawAuraIdle() {
        DrawCircleLines(Config::playerStartPos.x, Config::playerStartPos.y, AuraFieldSkill::AURA_RADIUS, {180, 0, 255, 30});
    }
    // Lingkaran ganda terang — aura aktif melindungi
    void drawAuraActive() {
        DrawCircleLines(Config::playerStartPos.x, Config::playerStartPos.y, AuraFieldSkill::AURA_RADIUS, {180, 0, 255, 150});
        DrawCircleLines(Config::playerStartPos.x, Config::playerStartPos.y, AuraFieldSkill::AURA_RADIUS + 4, {180, 0, 255, 80});
    }
}

AuraFieldSkill::AuraFieldSkill() {}

// Aktifkan aura: hanya bisa dari state IDLE
void AuraFieldSkill::activate() {
    if (state != IDLE) return;
    state = ACTIVE;
    timer = 0.0f;
}

// Update timer: ACTIVE → COOLDOWN → IDLE berdasarkan durasi konfigurasi
void AuraFieldSkill::update(float deltaTime) {
    if (state == ACTIVE) {
        timer += deltaTime;
        if (timer >= Config::auraFieldDuration) {
            state = COOLDOWN;
            timer = 0.0f;
        }
    } else if (state == COOLDOWN) {
        timer += deltaTime;
        if (timer >= Config::auraFieldCooldown) {
            state = IDLE;
            timer = 0.0f;
        }
    }
}

// Render lingkaran aura di sekitar player
// IDLE: lingkaran tipis transparan (indikasi siap)
// ACTIVE: lingkaran ganda terang (aura aktif)
void AuraFieldSkill::draw() {
    if (state == IDLE) {
        drawAuraIdle();
    } else if (state == ACTIVE) {
        drawAuraActive();
    }
}

const char* AuraFieldSkill::getName() const {
    return "AURA";
}

bool AuraFieldSkill::isReady() const {
    return state == IDLE;
}

bool AuraFieldSkill::isActive() const {
    return state == ACTIVE;
}

float AuraFieldSkill::getCooldownProgress() const {
    if (state == IDLE || state == ACTIVE) return 0.0f;
    return timer / Config::auraFieldCooldown;
}
