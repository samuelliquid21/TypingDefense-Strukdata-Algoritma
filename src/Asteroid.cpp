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
// Fungsi asteroidType menggunakan DEFAULT ARGUMENT (tier = 1)
// Bisa dipanggil sebagai asteroidType() atau asteroidType(3)
void Asteroid::asteroidType(const int tier) {

    // Set properti asteroid berdasarkan tier (1-6)
    // Tier 1-2: EASY, 3-4: MEDIUM, 5-6: HARD
    switch (tier) {
    case 1:
        word = WordSystem::getRandomWord(Difficulty::EASY);
        velocity.x = 50;  // Kecepatan rendah
        break;
    case 2:
        word = WordSystem::getRandomWord(Difficulty::EASY);
        velocity.x = 80;  // Kecepatan tinggi
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
        break;  // Default tier tidak valid
    }

    // Set properti lain yang tidak bergantung pada tier
    position.x = -radius;  // Start dari kiri layar
    position.y = GetRandomValue(0, Config::screenHeight);
    radius = 20 + (word.length() * 2);  // Radius berdasarkan panjang kata
    textureId = GetRandomValue(0, 3);   // Random texture
    active = true;    // Aktifkan asteroid
    targeted = false; // Belum ditarget
    
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

void Asteroid::draw() {
    DrawCircleV(position, radius, GRAY);
    DrawText(word.c_str(), position.x - radius, position.y - radius, 20, WHITE);

    if (targeted) {
        drawTargeted();
    }
}

void Asteroid::drawTargeted() {
    float size = radius * 2.2f;

    DrawRectangle(
        position.x - size/2,
        position.y - size/2,
        size,
        size,
        Fade(RED, 0.5f)
    );

    DrawText(
        "TARGETED",
        position.x - 45,
        position.y - radius - 10 - 25,
        20,
        RED
    );
}

int Asteroid::typingAsteroid(char characterTyped) {
    if (!active || word.empty()) return 0;
    if (characterTyped != word[0]) return 0;

    targeted = true;
    word.erase(0, 1);

    if (word.empty()) {
        active = false;
    }

return 1;
}


