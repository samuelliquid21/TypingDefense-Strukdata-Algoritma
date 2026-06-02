#pragma once
#include "Skill.h"

class ShieldSkill : public Skill {
public:
    ShieldSkill();
    void update(float deltaTime) override;
    void draw() override;
    const char* getName() const override;
    bool isReady() const override;
    bool isActive() const override;
    float getCooldownProgress() const override;
    void activate();             // aktivasi manual (tekan 1)
    bool consumeShield();        // true jika shield menyerap hit

    static constexpr int SHIELD_RADIUS = 52;   // radius lingkaran shield (collision)

private:
    enum State { IDLE, ACTIVE, COOLDOWN };
    State state = IDLE;
    float timer = 0.0f;
};
