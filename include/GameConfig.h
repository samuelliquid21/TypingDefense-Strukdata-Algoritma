#pragma once

#include "raylib.h"

namespace Config {
    constexpr int screenWidth{1080};
    constexpr int screenHeight{720};
    constexpr Vector2 playerStartPos{screenWidth - 80, screenHeight / 2};
    constexpr int playerHitbox{30};

    // Asteroid Manager Config
    constexpr float showerEventInterval{30};        // interval menambah event shower event
    constexpr float normalSpawnInterval{2.0f};      // interval meanmbah event asteroid biasa
    constexpr float executeEventInterval{2.0f};     // interval eksekusi event
    constexpr float addShowerNodeInterval{60.0f};   // interval menambah node asteroid baru di asteroid shower list
    constexpr float showerWaveInterval{0.2f};       // interval spawn antar asteroid saat event shower
    constexpr int initialShowerNodes{5};           // banyak asteroid pada event asteroid shower pertama
    constexpr bool enableAsteroidLog{true};         // memunculkan trace log asteroid

    // Shield Skill Config
    constexpr float shieldCooldown{30.0f};           // cooldown shield (detik)

    // Bomb Skill Config
    constexpr float bombCooldown{30.0f};             // cooldown bomb (detik)
    constexpr float bombShockwaveDuration{0.5f};     // durasi animasi shockwave (detik)
    constexpr float bombShockwaveMaxRadius{1100.0f};  // radius maksimal shockwave

    // Debug Config
    constexpr bool enableDebugPlayerInfo{true};      // tampilkan info player di menu utama
}