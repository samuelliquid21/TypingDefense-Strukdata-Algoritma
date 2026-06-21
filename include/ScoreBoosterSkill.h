#pragma once
#include "Skill.h"

// Skill Score Booster: multiplier 16x skor selama durasi aktif
class ScoreBoosterSkill : public Skill {
public:
    ScoreBoosterSkill();
    void update(float deltaTime) override;
    void draw() override;
    const char* getName() const override;
    bool isReady() const override;
    bool isActive() const override;
    float getCooldownProgress() const override;
    void activate();

    static constexpr int SCORE_MULTIPLIER = 16;

private:
    enum State { IDLE, ACTIVE, COOLDOWN };
    State state = IDLE;
    float timer = 0.0f;
};
