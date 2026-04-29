#pragma once
#include <string>
#include "raylib.h"

// Struct untuk representasi asteroid dalam game

struct Asteroid {

    // Properti asteroid dengan in-class initialization

    Vector2 position{0, 0};     
    Vector2 velocity{0, 0};     
    std::string word{};         
    int radius{0};              
    int textureId{0};           
    bool active{false};         
    bool targeted{false};       
    
    static int counter;  // Counter untuk total asteroid aktif (static member)

    // CONSTRUCTOR DAN DESTRUCTOR

    Asteroid();   // Constructor: increment counter
    ~Asteroid();  // Destructor: decrement counter

    // METHOD

    void asteroidType(const int tier = 1);  // Update properti asteroid sesuai tier (1-6). Menggunakan DEFAULT ARGUMENT: tier = 1 (EASY)
    int typingAsteroid(char characterTyped);  // Handle input untuk kata asteroid

    // GAME LOGIC
    
    void update(float deltaTime);  // Update posisi berdasarkan velocity
    void draw();                   // Render asteroid normal
    void drawTargeted();           // Render asteroid yang sedang ditarget (highlight)
};