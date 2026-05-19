#include "Asteroid.h"
#include "WordGenerator.h"
#include "GameConfig.h"
#include "raymath.h"

int Asteroid::counter = 0;

// ==== FUNGSI PEMBANTU ====

// menggunakan fungsi anonim agar fungsi ini tidak bisa digunakan di modul lain
namespace {

    float velocityYToPlayer(const Asteroid& asteroid) { // Menghitung kecepatan Y agar asteroid menuju pemain
        Vector2 direction = Vector2Subtract(Config::playerStartPos, asteroid.position);
        direction = Vector2Normalize(direction);
        return direction.y * fabsf(asteroid.velocity.x);
    }

    float randomVelocityY(const Asteroid& asteroid) {   // Menghitung kecepatan Y agar asteroid menuju pemain
        if (asteroid.position.y < Config::screenHeight / 2) {
            return GetRandomValue(5, 50); 
        } else {
            return GetRandomValue(-50, -5); 
        }
    }

}

Asteroid::Asteroid() { counter++; }
Asteroid::~Asteroid() { counter--; }

// ==== FUNGSI YANG DIBUTUHKAN DI MODUL LAIN ====

int Asteroid::typingAsteroid(char characterTyped) {     // Update properti asteroid sesuai tier (1-6). Dengan  DEFAULT ARGUMENT: tier = 1 (EASY)
    if (!active || word.empty()) return 0;
    if (characterTyped != word[0]) return 0;

    targeted = true;
    word.erase(0, 1);

    if (word.empty()) {
        active = false;
    }

    return 1;
}

void Asteroid::asteroidType(const int tier) {           // Handle input untuk kata asteroid
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

    position.x = -radius; 
    position.y = GetRandomValue(0, Config::screenHeight);
    radius = 20 + (word.length() * 2);  
    textureId = GetRandomValue(0, 3);  
    active = true;    
    targeted = false; 
    
    int toPlayer = GetRandomValue(0, 1); 
    if (toPlayer) {
        velocity.y = velocityYToPlayer(*this);
    } else {
        velocity.y = randomVelocityY(*this);
    }
}

// ==== UPDATE LOGIC ====

void Asteroid::update(float deltaTime) { // Update posisi berdasarkan velocity
    if (this->active) {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;

        if (position.x - radius > Config::screenWidth) {
            this->active = false;
        }
        if (position.x + radius < 0) {
            this->active = false;
        }
        if (position.y - radius > Config::screenHeight) {
            this->active = false;
        }
        if (position.y + radius < 0) {
            this->active = false;
        }
    }
}

// ==== DRAW LOGIC ====

void Asteroid::draw() {         // Render asteroid normal
    DrawCircleV(position, radius, GRAY);

    int textWidth = MeasureText(word.c_str(), 20);
    int textHeight = 20;
    DrawText(word.c_str(), position.x - textWidth/2, position.y - textHeight/2, 20, WHITE);

    if (targeted) {
        drawTargeted();
    }
}

void Asteroid::drawTargeted() { // Render asteroid yang sedang ditarget (highlight)
    float size = radius * 2.2f;

    DrawRectangle(
        position.x - size/2,
        position.y - size/2,
        size,
        size,
        Fade(RED, 0.5f)
    );

    const char* targetText = "TARGETED";
    int targetWidth = MeasureText(targetText, 20);
    DrawText(
        targetText,
        position.x - targetWidth/2,
        position.y - radius - 30,
        20,
        RED
    );
}

