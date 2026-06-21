#include "Background.h"
#include "AssetManager.h"

// ===============================
// 🌌 BACKGROUND SCROLLING
// ===============================

Background::Background() {
    scrollingBack = 0.0f;
    scrollSpeed = 0.0f;
    background = {};
}

// Load texture background dari file dan set kecepatan scroll
void Background::Load(const char* fileName, float speed) {
    background = AssetManager::getInstance().loadTexture("bg", fileName);
    scrollSpeed = speed;
}

void Background::Unload() {
    // no-op: AssetManager handle lifecycle
}

// Geser posisi scroll ke kiri, loop seamless jika sudah full width
void Background::Update() {
    scrollingBack += scrollSpeed * GetFrameTime();

    if (scrollingBack >= background.width) {
        scrollingBack = 0;
    }
}

// Render dua kopi background bersebelahan untuk efek scroll seamless
void Background::Draw() {
    DrawTextureEx(background, { scrollingBack, 0 }, 0.0f, 1.0f, WHITE);
    DrawTextureEx(background, { scrollingBack - (float)background.width, 0 }, 0.0f, 1.0f, WHITE);
}
