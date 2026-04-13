#include "Background.h"

Background::Background() {
    scrollingBack = 0.0f;
    scrollSpeed = 0.0f;
    background = {}; // Inisialisasi kosong
}

void Background::Load(const char* fileName, float speed) {
    // PENTING: LoadTexture hanya boleh dipanggil SETELAH InitWindow() di Game.cpp
    background = LoadTexture(fileName);
    scrollSpeed = speed;
}

void Background::Unload() {
    UnloadTexture(background);
}

void Background::Update() {
    scrollingBack += scrollSpeed * GetFrameTime();
    
    if (scrollingBack >= background.width) {
        scrollingBack = 0;
    }
}

void Background::Draw() {
    // Gambar utama
    DrawTextureEx(background, { scrollingBack, 0 }, 0.0f, 1.0f, WHITE);
    
    // Gambar pendukung di sebelah KIRI (mengisi kekosongan dari arah kiri)
    DrawTextureEx(background, { scrollingBack - (float)background.width, 0 }, 0.0f, 1.0f, WHITE);
}