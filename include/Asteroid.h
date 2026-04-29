#pragma once
#include <string>
#include "raylib.h"

// Struct untuk representasi asteroid dalam game
// Menggunakan in-class initialization untuk default values
struct Asteroid {
    // Properti asteroid dengan in-class initialization
    Vector2 position{0, 0};     // Posisi asteroid (x,y)
    Vector2 velocity{0, 0};     // Kecepatan asteroid (dx,dy)
    std::string word{};         // Kata yang harus diketik
    int radius{0};              // Radius asteroid untuk collision
    int textureId{0};           // ID texture asteroid
    bool active{false};         // Flag apakah asteroid aktif
    bool targeted{false};       // Flag apakah sedang ditarget player
    
    static int counter;  // Counter untuk total asteroid aktif (static member)

    // CONSTRUCTOR DAN DESTRUCTOR
    Asteroid();   // Constructor: increment counter
    ~Asteroid();  // Destructor: decrement counter

    // METHOD

    // Update properti asteroid sesuai tier (1-6)
    // Menggunakan DEFAULT ARGUMENT: tier = 1 (EASY)
    void asteroidType(const int tier = 1);  
    
    int typingAsteroid(char characterTyped);  // Handle input untuk kata asteroid

    // GAME LOGIC
    void update(float deltaTime);  // Update posisi berdasarkan velocity
    void draw();                   // Render asteroid normal
    void drawTargeted();           // Render asteroid yang sedang ditarget (highlight)
};