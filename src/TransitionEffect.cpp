#include "TransitionEffect.h"
#include "AudioManager.h"
#include "GameConfig.h"

// ===============================
// 🌀 EFEK TRANSISI GLITCH
// ===============================

void TransitionEffect::Update(float dt) {
    if (!m_active) return;

    m_timer -= dt;
    m_intensity = m_timer / m_duration;

    if (m_timer <= 0.0f) {
        m_active = false;
        m_intensity = 0.0f;
    }
}

// Gambar garis-garis glitch horizontal dengan warna bergantian cyan/magenta
static void drawGlitchLines(float intensity) {
    for (int i = 0; i < 12; i++) {
        int y = GetRandomValue(0, Config::screenHeight);
        int h = GetRandomValue(5, 25);
        Color glitchColor = (i % 2 == 0)
            ? Color{ 0, 255, 200, (unsigned char)(intensity * 180) }
            : Color{ 255, 50, 120, (unsigned char)(intensity * 140) };
        DrawRectangle(0, y, Config::screenWidth, h, glitchColor);
    }
}

// Gambar garis putih tipis tambahan sesekali (random 30% chance)
static void drawWhiteNoiseLine() {
    if (GetRandomValue(0, 10) > 7) {
        DrawRectangle(0, GetRandomValue(0, Config::screenHeight),
            Config::screenWidth, GetRandomValue(1, 3),
            Color{ 255, 255, 255, 100 });
    }
}

void TransitionEffect::Draw() const {
    if (!m_active || m_intensity <= 0.01f) return;

    drawGlitchLines(m_intensity);
    drawWhiteNoiseLine();
}

void TransitionEffect::Start() {
    m_timer = m_duration;
    m_active = true;
    m_intensity = 1.0f;
}

bool TransitionEffect::IsActive() const {
    return m_active;
}

void TransitionEffect::PlaySoundIn() {
    AudioManager::getInstance().playSfxOnce("glitchMasuk");
}

void TransitionEffect::PlaySoundOut() {
    AudioManager::getInstance().playSfxOnce("glitchKeluar");
}
