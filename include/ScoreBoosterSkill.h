#pragma once
#include "Skill.h"

// Skill Score Booster: multiplier 16x skor selama durasi aktif (5 detik).
// Cooldown 30 detik. Stack dengan combo multiplier untuk efek maksimal.
class ScoreBoosterSkill : public Skill {
public:
    ScoreBoosterSkill();
    void update(float deltaTime) override;  // Update timer state machine
    void draw() override;                   // Render lingkaran emas di sekitar player
    const char* getName() const override;
    bool isReady() const override;
    bool isActive() const override;
    float getCooldownProgress() const override;
    void activate() override;               // Aktifkan skill: IDLE → ACTIVE

    static constexpr int SCORE_MULTIPLIER = 16; // Faktor multiplier skor

private:
    enum State { IDLE, ACTIVE, COOLDOWN };  // State machine skill
    State state = IDLE;
    float timer = 0.0f;     // Timer untuk durasi ACTIVE dan COOLDOWN
};
