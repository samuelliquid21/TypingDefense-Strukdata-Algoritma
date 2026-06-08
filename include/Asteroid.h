#pragma once
#include <string>
#include "raylib.h"

// Struktur data asteroid: menyimpan properti posisi, kata, dan status
struct Asteroid {

    // ==== PROPERTI ASTEROID ====

    Vector2 position{0, 0};     // Posisi asteroid di layar
    Vector2 velocity{0, 0};     // Kecepatan gerak asteroid (x horizontal, y vertikal)
    std::string word{};         // Sisa kata yang belum diketik (berkurang saat diketik)
    std::string originalWord{}; // Kata asli sebelum diketik (untuk callback)
    int radius{0};              // Radius lingkaran asteroid (berdasar panjang kata)
    int textureId{0};           // ID tekstur untuk variasi visual (0-3)
    bool active{false};         // Status aktif: true = tampil dan bisa di-interaksi
    bool targeted{false};       // Status sedang ditarget oleh pemain

    static int counter; // Counter global jumlah objek Asteroid yang hidup

    // ==== CONSTRUCTOR DAN DESTRUCTOR ====

    Asteroid();   // Constructor: increment static counter
    ~Asteroid();  // Destructor: decrement static counter

    // ==== FUNGSI UTAMA ====

    void asteroidType(const int tier = 1);      // Set properti berdasarkan tier (1-6), aktifkan asteroid
    int typingAsteroid(char characterTyped);    // Proses input karakter untuk mengetik kata asteroid

    void update(float deltaTime);  // Update posisi berdasarkan velocity
    void draw();                   // Render asteroid normal (+ highlight jika ditarget)
    void drawTargeted();           // Render highlight kotak merah untuk asteroid yang ditarget
};
