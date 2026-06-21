#pragma once
#include "Skill.h"

// Skill Aura Field: shield aktif berdasarkan durasi (10 detik)
class AuraFieldSkill : public Skill {
public:
    AuraFieldSkill();
    void update(float deltaTime) override;
    void draw() override;
    const char* getName() const override;
    bool isReady() const override;
    bool isActive() const override;
    float getCooldownProgress() const override;
    void activate() override;
    static constexpr int AURA_RADIUS = 64;

private:
    enum State { IDLE, ACTIVE, COOLDOWN };
    State state = IDLE;
    float timer = 0.0f;
};
