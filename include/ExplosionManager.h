#pragma once
#include "raylib.h"

// Satu partikel dalam ledakan: posisi, kecepatan, radius
struct Particle {
    Vector2 pos;
    Vector2 vel;
    float radius;
};

// Satu efek ledakan: kumpulan partikel yang bergerak dan memudar
struct Explosion {
    Vector2 pos{0, 0};
    float timer{0};
    float duration{0.6f};
    Particle particles[35];
    int particleCount{0};
    Color color{ORANGE};
    bool active{false};
};

// Manager partikel: mengelola pool ledakan, spawn, update, draw
class ExplosionManager {
public:
    void spawn(Vector2 pos, int count, Color color);
    void update(float dt);
    void draw();

private:
    static const int MAX_EXPLOSIONS = 64;
    Explosion m_pool[MAX_EXPLOSIONS];
};
