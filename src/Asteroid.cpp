#include "Asteroid.h"
#include "WordGenerator.h"
#include "GameConfig.h"
#include "raymath.h"

namespace {
    // Menghitung kecepatan Y agar asteroid menuju pemain
    float velocityYToPlayer(const Asteroid& asteroid) {
        Vector2 direction = Vector2Subtract(Config::playerStartPos, asteroid.position);
        direction = Vector2Normalize(direction);
        return direction.y * fabsf(asteroid.velocity.x);
    }

    // Menghasilkan kecepatan Y acak untuk asteroid yang tidak langsung menuju pemain
    float randomVelocityY(const Asteroid& asteroid) {
        if (asteroid.position.y < Config::screenHeight / 2) {
            return GetRandomValue(5, 50); 
        } else {
            return GetRandomValue(-50, -5); 
        }
    }
}

int Asteroid::counter = 0;

// CONSTRUCTOR DAN DESTRUCTOR
Asteroid::Asteroid() { counter++; }
Asteroid::~Asteroid() { counter--; }

// METHOD
void Asteroid::asteroidType(const int tier) {

    // Set properti asteroid berdasarkan tier
    switch (tier) {
    case 1:
        word = WordSystem::getRandomWord(Difficulty::EASY);
        velocity.x = 50;
        break;
    case 2:
        word = WordSystem::getRandomWord(Difficulty::EASY);
        velocity.x = 80;
        break;
    case 3:
        word = WordSystem::getRandomWord(Difficulty::MEDIUM);
        velocity.x = 50;
        break;
    case 4:
        word = WordSystem::getRandomWord(Difficulty::MEDIUM);
        velocity.x = 80;
        break;
    case 5:
        word = WordSystem::getRandomWord(Difficulty::HARD);
        velocity.x = 50;
        break;
    case 6:
        word = WordSystem::getRandomWord(Difficulty::HARD);
        velocity.x = 80;
        break;
    default:
        break;
    }

    // Set properti lain yang tidak bergantung pada tier
    position.x = -radius;
    position.y = GetRandomValue(0, Config::screenHeight);
    wordIndex = 0;
    radius = 20 + (word.length() * 2);
    textureId = GetRandomValue(0, 3);
    active = true;
    targeted = false;
    
    // Tentukan apakah asteroid akan langsung menuju pemain atau tidak
    int toPlayer = GetRandomValue(0, 1); 
    if (toPlayer) {
        velocity.y = velocityYToPlayer(*this);
    } else {
        velocity.y = randomVelocityY(*this);
    }
}

// GAME LOGIC
void Asteroid::update(float deltaTime) {
    if (this->active) {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        if (position.x - radius > Config::screenWidth) {
            this->active = false;
        }
    }
}
void Asteroid::draw() {
    DrawCircleV(position, radius, GRAY);
    DrawText(word.c_str(), position.x - radius, position.y - radius, 20, BLACK);
}


