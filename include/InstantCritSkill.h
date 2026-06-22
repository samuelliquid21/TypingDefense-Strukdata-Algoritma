#pragma once
#include "Skill.h"

// Skill Instant Crit: huruf pertama yang diketik langsung menghancurkan
// asteroid target. Aktif selama 10 detik, cooldown 30 detik.
class InstantCritSkill : public Skill {
public:
    InstantCritSkill();
    void update(float deltaTime) override;  // Update timer state machine
    void draw() override;                   // Render indikator kotak merah di sekitar player
    const char* getName() const override;
    bool isReady() const override;
    bool isActive() const override;
    float getCooldownProgress() const override;
    void activate() override;               // Aktifkan skill: IDLE → ACTIVE

private:
    enum State { IDLE, ACTIVE, COOLDOWN };  // State machine skill
    State state = IDLE;
    float timer = 0.0f;     // Timer untuk durasi ACTIVE dan COOLDOWN
};
