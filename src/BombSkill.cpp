#include "BombSkill.h"
#include "raylib.h"
#include "GameConfig.h"

BombSkill::BombSkill() {}

bool BombSkill::activate() {
    if (state != IDLE) return false;
    state = EXPLODING;
    timer = 0.0f;
    shockwaveRadius = 0.0f;
    return true;
}

void BombSkill::update(float deltaTime) {
    if (state == EXPLODING) {
        timer += deltaTime;
        shockwaveRadius = (timer / Config::bombShockwaveDuration) * Config::bombShockwaveMaxRadius;
        if (timer >= Config::bombShockwaveDuration) {
            state = COOLDOWN;
            timer = 0.0f;
        }
    } else if (state == COOLDOWN) {
        timer += deltaTime;
        if (timer >= Config::bombCooldown) {
            state = IDLE;
            timer = 0.0f;
        }
    }
}

void BombSkill::draw() {
    if (state != EXPLODING) return;

    float alpha = 1.0f - (timer / Config::bombShockwaveDuration);
    Color c1 = {255, 255, 255, (unsigned char)(alpha * 180)};
    Color c2 = {0, 200, 255, (unsigned char)(alpha * 80)};

    DrawCircleLines(
        Config::playerStartPos.x,
        Config::playerStartPos.y,
        shockwaveRadius,
        c1
    );
    DrawCircleLines(
        Config::playerStartPos.x,
        Config::playerStartPos.y,
        shockwaveRadius * 0.8f,
        c2
    );
}

const char* BombSkill::getName() const {
    return "Bomb";
}

bool BombSkill::isReady() const {
    return state == IDLE;
}

bool BombSkill::isActive() const {
    return state == EXPLODING;
}

float BombSkill::getShockwaveRadius() const {
    return shockwaveRadius;
}

float BombSkill::getCooldownProgress() const {
    if (state == IDLE) return 0.0f;
    if (state == EXPLODING) return 0.0f;
    return timer / Config::bombCooldown;
}
