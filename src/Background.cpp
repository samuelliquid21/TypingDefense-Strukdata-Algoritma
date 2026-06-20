#include "Background.h"
#include "AssetManager.h"

Background::Background() {
    scrollingBack = 0.0f;
    scrollSpeed = 0.0f;
    background = {}; // Inisialisasi kosong
}

void Background::Load(const char* fileName, float speed) {
    background = AssetManager::getInstance().loadTexture("bg", fileName);
    scrollSpeed = speed;
}

void Background::Unload() {
    // no-op: AssetManager handle lifecycle
}

void Background::Update() {
    // Geser posisi scroll ke kiri berdasarkan kecepatan dan delta time
    scrollingBack += scrollSpeed * GetFrameTime();

    // Reset posisi jika sudah melebihi lebar tekstur (loop seamless)
    if (scrollingBack >= background.width) {
        scrollingBack = 0;
    }
}

void Background::Draw() {
    // Gambar utama pada posisi scroll saat ini
    DrawTextureEx(background, { scrollingBack, 0 }, 0.0f, 1.0f, WHITE);

    // Gambar pendukung di sebelah KIRI untuk mengisi celah saat scrolling
    DrawTextureEx(background, { scrollingBack - (float)background.width, 0 }, 0.0f, 1.0f, WHITE);
}
