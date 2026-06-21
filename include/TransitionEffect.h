#pragma once
#include "raylib.h"

// Efek transisi glitch antar state game.
// Menampilkan garis-garis horizontal acak (glitch scanlines) dengan warna
// cyan dan magenta selama durasi tertentu, lalu memudar. Tidak lagi
// menahan state change — murni visual overlay.
class TransitionEffect {
public:
    TransitionEffect() = default;
    ~TransitionEffect() = default;

    void Update(float dt);  // Update timer dan intensity
    void Draw() const;      // Render glitch scanlines

    void Start();           // Mulai efek transisi (reset timer)

    bool IsActive() const;  // Apakah efek transisi sedang berjalan

    void PlaySoundIn();     // Mainkan SFX glitch masuk
    void PlaySoundOut();    // Mainkan SFX glitch keluar

private:
    float m_timer = 0.0f;      // Sisa waktu transisi (detik)
    float m_duration = 0.6f;   // Durasi total transisi
    bool m_active = false;     // Status aktif
    float m_intensity = 0.0f;  // Intensitas visual (1.0 → 0.0)
};
