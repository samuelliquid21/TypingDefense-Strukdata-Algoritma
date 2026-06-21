#pragma once
#include "Skill.h"

// Skill Instant Crit: huruf pertama langsung hancurkan asteroid (10 detik)
class InstantCritSkill : public Skill {
public:
    InstantCritSkill();
    void update(float deltaTime) override;
    void draw() override;
    const char* getName() const override;
    bool isReady() const override;
    bool isActive() const override;
    float getCooldownProgress() const override;
    void activate() override;

private:
    enum State { IDLE, ACTIVE, COOLDOWN };
    State state = IDLE;
    float timer = 0.0f;
};
