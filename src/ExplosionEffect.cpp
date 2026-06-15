#include "ExplosionEffect.h"
#include <algorithm>
#include <cmath>

ExplosionEffect::ExplosionEffect()
    : active(false), timer(0.0f), duration(1.2f), origin({0, 0}) {}

ExplosionEffect::~ExplosionEffect() {}

void ExplosionEffect::Trigger(Vector2 position, float dur) {
    origin = position;
    duration = dur;
    timer = duration;
    active = true;

    SpawnParticles();
    SpawnSparks();
}

void ExplosionEffect::SpawnParticles() {
    particles.clear();
    const int count = 35;

    for (int i = 0; i < count; i++) {
        ExplosionParticle p;
        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        float speed = (float)GetRandomValue(60, 250);

        p.pos = origin;
        p.pos.x += (float)GetRandomValue(-10, 10);
        p.pos.y += (float)GetRandomValue(-10, 10);
        p.velocity = { cosf(angle) * speed, sinf(angle) * speed };
        p.radius = (float)GetRandomValue(3, 12);
        p.life = (float)GetRandomValue(40, 100) / 100.0f;

        int colorType = GetRandomValue(0, 5);
        switch (colorType) {
            case 0: p.color = { 255, 220, 50, 255 }; break;
            case 1: p.color = { 255, 140, 30, 255 }; break;
            case 2: p.color = { 255, 60, 40, 255 }; break;
            case 3: p.color = { 255, 255, 180, 255 }; break;
            case 4: p.color = { 200, 80, 20, 255 }; break;
            case 5: p.color = { 255, 180, 60, 255 }; break;
        }
        p.alpha = 1.0f;

        particles.push_back(p);
    }
}

void ExplosionEffect::SpawnSparks() {
    sparks.clear();
    const int count = 15;

    for (int i = 0; i < count; i++) {
        ExplosionParticle s;
        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        float speed = (float)GetRandomValue(100, 400);

        s.pos = origin;
        s.velocity = { cosf(angle) * speed, sinf(angle) * speed };
        s.radius = (float)GetRandomValue(1, 3);
        s.life = (float)GetRandomValue(20, 60) / 100.0f;
        s.color = { 255, 255, 255, 255 };
        s.alpha = 1.0f;

        sparks.push_back(s);
    }
}

void ExplosionEffect::Update(float dt) {
    if (!active) return;

    timer -= dt;
    UpdateParticles(dt);

    if (timer <= 0.0f) {
        active = false;
        particles.clear();
        sparks.clear();
    }
}

void ExplosionEffect::UpdateParticles(float dt) {
    for (auto& p : particles) {
        p.pos.x += p.velocity.x * dt;
        p.pos.y += p.velocity.y * dt;
        p.velocity.x *= 0.96f;
        p.velocity.y *= 0.96f;
        p.alpha = fmaxf(0.0f, p.alpha - dt * (1.5f / p.life));
        p.radius = fmaxf(0.5f, p.radius - dt * 10.0f);
    }

    for (auto& s : sparks) {
        s.pos.x += s.velocity.x * dt;
        s.pos.y += s.velocity.y * dt;
        s.velocity.x *= 0.93f;
        s.velocity.y *= 0.93f;
        s.alpha = fmaxf(0.0f, s.alpha - dt * (2.0f / s.life));
    }

    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const ExplosionParticle& p) { return p.alpha <= 0.0f || p.radius <= 0.5f; }),
        particles.end());

    sparks.erase(
        std::remove_if(sparks.begin(), sparks.end(),
            [](const ExplosionParticle& s) { return s.alpha <= 0.0f; }),
        sparks.end());
}

void ExplosionEffect::Draw() {
    if (!active) return;
    DrawParticles();
}

void ExplosionEffect::DrawParticles() {
    // Draw glow first (bigger, transparent)
    for (auto& p : particles) {
        unsigned char a = (unsigned char)(p.alpha * 80);
        DrawCircleV(p.pos, p.radius * 3.0f,
            { p.color.r, p.color.g, p.color.b, a });
    }

    // Draw main particles
    for (auto& p : particles) {
        unsigned char a = (unsigned char)(p.alpha * 255);
        DrawCircleV(p.pos, p.radius,
            { p.color.r, p.color.g, p.color.b, a });
    }

    // Draw sparks
    for (auto& s : sparks) {
        unsigned char a = (unsigned char)(s.alpha * 255);
        DrawCircleV(s.pos, s.radius, { 255, 255, 255, a });
    }
}
