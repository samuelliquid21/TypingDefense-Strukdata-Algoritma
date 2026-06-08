#pragma once

// Interface abstrak untuk semua skill aktif (Shield, Bomb, dll.)
class Skill {
public:
    virtual ~Skill() = default;
    virtual void update(float deltaTime) = 0;             // Update cooldown & state tiap frame
    virtual void draw() = 0;                               // Render visual skill
    virtual const char* getName() const = 0;               // Nama skill (untuk UI)
    virtual bool isReady() const = 0;                      // Apakah skill siap dipakai? (IDLE)
    virtual bool isActive() const = 0;                     // Apakah skill sedang aktif?
    virtual float getCooldownProgress() const = 0;         // Progress cooldown 0.0–1.0
};
