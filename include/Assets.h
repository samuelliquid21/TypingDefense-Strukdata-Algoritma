#include "raylib.h"

class Sprite {
private:
    Texture2D* textureSource; // Hanya menyimpan alamat memory texture (hemat RAM)
    Rectangle sourceRect;
    Vector2 origin;

public:
    Vector2 position;
    float rotation;
    float scale;
    Color tint;

    // Constructor: Menerima pointer texture, koordinat potong, dan jumlah frame
    Sprite(Texture2D* tex, float srcX, float srcY, float width, float height) {
        textureSource = tex;
        sourceRect = { srcX, srcY, width, height };
        
        // Default values
        position = { 0, 0 };
        rotation = 0.0f;
        scale = 1.0f;
        tint = WHITE;
        
        // Pivot otomatis di tengah sprite
        origin = { width / 2.0f, height / 2.0f };
    }

    // Mengubah potongan gambar (misal untuk animasi manual)
    void SetSource(float x, float y) {
        sourceRect.x = x;
        sourceRect.y = y;
    }

    // Fungsi Render yang rapi
    void Draw() {
        // Rectangle tujuan (posisi di layar & scaling)
        Rectangle destRect = { 
            position.x, 
            position.y, 
            sourceRect.width * scale, 
            sourceRect.height * scale 
        };

        // DrawTexturePro adalah fungsi paling modular di Raylib
        DrawTexturePro(*textureSource, sourceRect, destRect, 
                       {origin.x * scale, origin.y * scale}, 
                       rotation, tint);
    }
};