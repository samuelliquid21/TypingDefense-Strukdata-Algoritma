#include "ShieldSkill.h"
#include "raylib.h"
#include "GameConfig.h"

ShieldSkill::ShieldSkill() {}

void ShieldSkill::activate() {
    if (state != IDLE) return;
    state = ACTIVE;
    timer = 0.0f;
}

void ShieldSkill::update(float deltaTime) {
    if (state == COOLDOWN) {
        timer += deltaTime;
        if (timer >= Config::shieldCooldown) {
            state = IDLE;
            timer = 0.0f;
        }
    }
}

void ShieldSkill::draw() {
    if (state == IDLE) {
        // lingkaran tipis — shield siap dipakai
        DrawCircleLines(
            Config::playerStartPos.x,
            Config::playerStartPos.y,
            48,
            {0, 200, 255, 30}
        );
    } else if (state == ACTIVE) {
        // lingkaran terang — shield aktif
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
    // COOLDOWN: tidak ada visual
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

float ShieldSkill::getCooldownProgress() const {
    if (state == IDLE || state == ACTIVE) return 0.0f;
    return timer / Config::shieldCooldown;
}

bool ShieldSkill::consumeShield() {
    if (state != ACTIVE) return false;
    state = COOLDOWN;
    timer = 0.0f;
    return true;
}
