#include "InstantCritSkill.h"
#include "raylib.h"
#include "GameConfig.h"

// ===============================
// ⚡ INSTANT CRIT SKILL
// ===============================

InstantCritSkill::InstantCritSkill() {}

// Aktifkan instant crit: hanya bisa dari state IDLE
void InstantCritSkill::activate() {
    if (state != IDLE) return;
    state = ACTIVE;
    timer = 0.0f;
}

// Update timer: ACTIVE → COOLDOWN → IDLE berdasarkan durasi konfigurasi
void InstantCritSkill::update(float deltaTime) {
    if (state == ACTIVE) {
        timer += deltaTime;
        if (timer >= Config::instantCritDuration) {
            state = COOLDOWN;
            timer = 0.0f;
        }
    } else if (state == COOLDOWN) {
        timer += deltaTime;
        if (timer >= Config::instantCritCooldown) {
            state = IDLE;
            timer = 0.0f;
        }
    }
}

// Render indikator kotak merah di sekitar player
// IDLE: garis tipis transparan
// ACTIVE: garis ganda terang
void InstantCritSkill::draw() {
    if (state == IDLE) {
        DrawRectangleLines(
            Config::playerStartPos.x - 32,
            Config::playerStartPos.y - 32,
            64, 64,
            {255, 60, 60, 30}
        );
    } else if (state == ACTIVE) {
        DrawRectangleLines(
            Config::playerStartPos.x - 32,
            Config::playerStartPos.y - 32,
            64, 64,
            {255, 60, 60, 180}
        );
        DrawRectangleLines(
            Config::playerStartPos.x - 36,
            Config::playerStartPos.y - 36,
            72, 72,
            {255, 60, 60, 80}
        );
    }
}

const char* InstantCritSkill::getName() const {
    return "CRIT";
}

bool InstantCritSkill::isReady() const {
    return state == IDLE;
}

bool InstantCritSkill::isActive() const {
    return state == ACTIVE;
}

float InstantCritSkill::getCooldownProgress() const {
    if (state == IDLE || state == ACTIVE) return 0.0f;
    return timer / Config::instantCritCooldown;
}
