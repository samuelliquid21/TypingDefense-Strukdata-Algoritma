#pragma once
#include "Skill.h"

// Skill Bomb: ledakan shockwave yang menghancurkan asteroid dalam radius
class BombSkill : public Skill {
public:
    BombSkill();
    void update(float deltaTime) override;        // Update animasi shockwave & cooldown
    void draw() override;                          // Render lingkaran shockwave
    const char* getName() const override;
    bool isReady() const override;
    bool isActive() const override;
    float getCooldownProgress() const override;
    void activate() override;                               // picu ledakan
    float getShockwaveRadius() const;              // Radius shockwave saat ini (untuk deteksi collision)

private:
    // State machine: IDLE → EXPLODING → COOLDOWN → IDLE
    enum State { IDLE, EXPLODING, COOLDOWN };
    State state = IDLE;
    float timer = 0.0f;             // Timer untuk durasi EXPLODING atau COOLDOWN
    float shockwaveRadius = 0.0f;   // Radius shockwave yang membesar selama EXPLODING
};
