#pragma once

#include "raylib.h"

namespace Config {
    constexpr int screenWidth{1080};
    constexpr int screenHeight{720};
    constexpr int maxAsteroids{100};
    constexpr Vector2 playerStartPos{screenWidth - 80, screenHeight / 2};
    constexpr int playerHitbox{30};
}