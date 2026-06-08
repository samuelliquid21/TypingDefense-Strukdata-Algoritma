#pragma once
#include "Skill.h"

// Skill Shield: menyerap 1 hantaman asteroid, dengan cooldown 30 detik
class ShieldSkill : public Skill {
public:
    ShieldSkill();
    void update(float deltaTime) override;         // Update timer cooldown tiap frame
    void draw() override;                           // Render lingkaran shield di sekitar player
    const char* getName() const override;
    bool isReady() const override;
    bool isActive() const override;
    float getCooldownProgress() const override;     // 0.0 = siap, 1.0 = cooldown selesai
    void activate();                                // Aktivasi manual (tekan tombol 1)
    bool consumeShield();                           // True jika shield menyerap hit → masuk COOLDOWN

    static constexpr int SHIELD_RADIUS = 52;        // Radius lingkaran shield untuk deteksi collision

private:
    // State machine: IDLE → ACTIVE → COOLDOWN → IDLE
    enum State { IDLE, ACTIVE, COOLDOWN };
    State state = IDLE;
    float timer = 0.0f;  // Timer untuk durasi ACTIVE atau COOLDOWN
};
