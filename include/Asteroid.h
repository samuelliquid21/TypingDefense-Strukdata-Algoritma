#pragma once
#include <string>
#include "json.hpp"
#include "raylib.h"

struct Asteroid {
    // Properti asteroid dan nilai defaultnya
    Vector2 position{0, 0};
    Vector2 velocity{0, 0};
    std::string word{};
    int wordIndex{0};
    int radius{0};
    int textureId{0};
    bool active{false};
    bool targeted{false};
    
    static int counter;

    // CONSTRUCTOR DAN DESTRUCTOR
    Asteroid();
    ~Asteroid();

    // METHOD

    // Update properti asteroid sesuai dengan tier. @param tier: tingkat kesulitan asteroid (1 sampai 10)
    void asteroidType(const int tier);  

    // GAME LOGIC
    void update(float deltaTime);
    void draw();
};