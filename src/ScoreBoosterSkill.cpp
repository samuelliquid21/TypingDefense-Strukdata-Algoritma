#include "ScoreBoosterSkill.h"
#include "raylib.h"
#include "GameConfig.h"

ScoreBoosterSkill::ScoreBoosterSkill() {}

void ScoreBoosterSkill::activate() {
    if (state != IDLE) return;
    state = ACTIVE;
    timer = 0.0f;
}

void ScoreBoosterSkill::update(float deltaTime) {
    if (state == ACTIVE) {
        timer += deltaTime;
        if (timer >= Config::scoreBoosterDuration) {
            state = COOLDOWN;
            timer = 0.0f;
        }
    } else if (state == COOLDOWN) {
        timer += deltaTime;
        if (timer >= Config::scoreBoosterCooldown) {
            state = IDLE;
            timer = 0.0f;
        }
    }
}

void ScoreBoosterSkill::draw() {
    if (state == IDLE) {
        // Lingkaran tipis emas — indikasi siap
        DrawCircleLines(
            Config::playerStartPos.x,
            Config::playerStartPos.y,
            64,
            {255, 200, 0, 30}
        );
    } else if (state == ACTIVE) {
        // Lingkaran emas terang — multiplier aktif
        DrawCircleLines(
            Config::playerStartPos.x,
            Config::playerStartPos.y,
            64,
            {255, 220, 0, 150}
        );
        DrawCircleLines(
            Config::playerStartPos.x,
            Config::playerStartPos.y,
            68,
            {255, 200, 0, 80}
        );
    }
}

const char* ScoreBoosterSkill::getName() const {
    return "Score Booster";
}

bool ScoreBoosterSkill::isReady() const {
    return state == IDLE;
}

bool ScoreBoosterSkill::isActive() const {
    return state == ACTIVE;
}

float ScoreBoosterSkill::getCooldownProgress() const {
    if (state == IDLE || state == ACTIVE) return 0.0f;
    return timer / Config::scoreBoosterCooldown;
}
