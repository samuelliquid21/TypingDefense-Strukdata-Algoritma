#pragma once
#include "raylib.h"

// Kelas untuk mengelola background scrolling horizontal.
// Menggambar tekstur yang sama secara seamless dengan dua instance
// yang bersebelahan untuk menciptakan efek infinite scroll.

class Background {
private:
    Texture2D background;
    float scrollingBack;
    float scrollSpeed;

public:
    Background();

    // Memuat tekstur dari file dan mengatur kecepatan scroll
    void Load(const char* fileName, float speed);
    // Membebaskan memori tekstur
    void Unload();
    // Menggeser posisi scroll berdasarkan delta time
    void Update();
    // Merender background dengan dua buah tekstur yang saling menyambung
    void Draw();
};
