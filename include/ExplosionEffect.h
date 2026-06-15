#pragma once
#include "raylib.h"
#include <vector>

struct ExplosionParticle {
    Vector2 pos;
    Vector2 velocity;
    float radius;
    Color color;
    float alpha;
    float life;
};

class ExplosionEffect {
public:
    ExplosionEffect();
    ~ExplosionEffect();

    void Trigger(Vector2 position, float duration = 1.2f);
    void Update(float dt);
    void Draw();
    bool IsActive() const { return active; }

private:
    bool active;
    float timer;
    float duration;
    Vector2 origin;

    std::vector<ExplosionParticle> particles;
    std::vector<ExplosionParticle> sparks;

    void SpawnParticles();
    void SpawnSparks();
    void UpdateParticles(float dt);
    void DrawParticles();
};
