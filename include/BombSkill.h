#pragma once
#include "Skill.h"

class BombSkill : public Skill {
public:
    BombSkill();
    void update(float deltaTime) override;
    void draw() override;
    const char* getName() const override;
    bool isReady() const override;
    bool isActive() const override;
    float getCooldownProgress() const override;
    bool activate();   // true jika bom meledak
    float getShockwaveRadius() const;

private:
    enum State { IDLE, EXPLODING, COOLDOWN };
    State state = IDLE;
    float timer = 0.0f;
    float shockwaveRadius = 0.0f;
};
