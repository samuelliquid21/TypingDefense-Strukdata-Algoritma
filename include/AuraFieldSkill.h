#pragma once
#include "Skill.h"

// Skill Aura Field: shield aktif berdasarkan durasi (10 detik).
// Saat aktif, semua asteroid yang mendekati player otomatis hancur.
// Tidak bisa dikonsumsi per-hit — bertahan penuh selama durasi.
class AuraFieldSkill : public Skill {
public:
    AuraFieldSkill();
    void update(float deltaTime) override;  // Update timer state ACTIVE → COOLDOWN → IDLE
    void draw() override;                   // Render lingkaran aura di sekitar player
    const char* getName() const override;
    bool isReady() const override;
    bool isActive() const override;
    float getCooldownProgress() const override;
    void activate() override;               // Aktifkan skill: IDLE → ACTIVE

    static constexpr int AURA_RADIUS = 64;  // Radius visual lingkaran aura

private:
    enum State { IDLE, ACTIVE, COOLDOWN };  // State machine skill
    State state = IDLE;
    float timer = 0.0f;     // Timer untuk durasi ACTIVE dan COOLDOWN
};
