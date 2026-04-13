#pragma once
#include "raylib.h"

class Background {
private:
    Texture2D background;
    float scrollingBack;
    float scrollSpeed;

public:
    // Constructor default
    Background();
    
    // Fungsi utama
    void Load(const char* fileName, float speed);
    void Unload();
    void Update();
    void Draw();
};