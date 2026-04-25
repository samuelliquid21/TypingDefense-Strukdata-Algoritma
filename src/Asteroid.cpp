#include "Asteroid.h"
#include "WordGenerator.h"
#include "GameConfig.h"
#include "raymath.h"

namespace {
    float velocityYToPlayer(const Asteroid& asteroid) {
        Vector2 direction = Vector2Subtract(Config::playerStartPos, asteroid.position);
        float length = Vector2Length(direction);

        if (length > 0.0f) {
            direction = Vector2Scale(direction, 1.0f / length); // normalize

            float speedX = asteroid.velocity.x;

            // jaga proporsi arah → supaya tidak lebih cepat dari X
            return direction.y * fabs(speedX);
        }

        return 0.0f;
    }

    // Menghasilkan kecepatan Y acak untuk asteroid yang tidak langsung menuju pemain
    float randomVelocityY(const Asteroid& asteroid) {
        if (asteroid.position.y < Config::screenHeight / 2) {
            return GetRandomValue(20, 50); // ke bawah
        } else {
            return GetRandomValue(-50, -20); // ke atas
        }
    }
}

int Asteroid::counter = 0;

// CONSTRUCTOR DAN DESTRUCTOR
Asteroid::Asteroid() { counter++; }
Asteroid::~Asteroid() { counter--; }

// METHOD
// void Asteroid::asteroidType(const int tier) {
//     int toPlayer = GetRandomValue(0, 1); 
//     if (toPlayer) {
//         velocity.y = velocityYToPlayer(*this);
//     } else {
//         velocity.y = randomVelocityY(*this);
//     }
// }

// GAME LOGIC
void Asteroid::update(float deltaTime) {
    if (this->active) {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        if (position.x - radius > Config::screenWidth) {
            this->active = false;
        }
        // tesitng: targeting asteroid
        if (IsKeyDown(KEY_A)) {
            this->targeted = true;
        } else {
            this->targeted = false;
        }
    }
}
void Asteroid::draw() {
    if (this->active) {
        DrawCircleV(position, radius, GRAY);
        if (this->targeted) {
            // draw kotak merah transparan di sekitar asteroid yang ditargetkan
            DrawRectangleV({position.x - radius - 5.0f, position.y - radius - 5.0f}, {radius * 2.0f + 10.0f, radius * 2.0f + 10.0f}, (Color){255, 0, 0, 100});
        }
        DrawTextEx(GetFontDefault(), word.c_str(), {position.x - radius, position.y - radius}, 20, 1, WHITE);
    }
}


