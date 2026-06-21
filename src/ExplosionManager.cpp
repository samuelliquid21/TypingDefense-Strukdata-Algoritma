#include "ExplosionManager.h"
#include "raymath.h"

void ExplosionManager::spawn(Vector2 pos, int count, Color color) {
    // Cari slot pool yang tidak aktif
    for (auto &e : m_pool) {
        if (!e.active) {
            e.pos = pos;
            e.color = color;
            e.timer = e.duration;
            e.particleCount = (count > 35) ? 35 : count;
            e.active = true;

            for (int i = 0; i < e.particleCount; i++) {
                float angle = GetRandomValue(0, 360) * DEG2RAD;
                float speed = (float)GetRandomValue(80, 300);
                e.particles[i].pos = pos;
                e.particles[i].vel = {
                    cosf(angle) * speed,
                    sinf(angle) * speed
                };
                e.particles[i].radius = (float)GetRandomValue(3, 8);
            }
            return;
        }
    }
}

void ExplosionManager::update(float dt) {
    for (auto &e : m_pool) {
        if (!e.active) continue;

        e.timer -= dt;
        for (int i = 0; i < e.particleCount; i++) {
            e.particles[i].pos.x += e.particles[i].vel.x * dt;
            e.particles[i].pos.y += e.particles[i].vel.y * dt;
            // Damping perlambatan
            e.particles[i].vel.x *= 0.98f;
            e.particles[i].vel.y *= 0.98f;
        }

        if (e.timer <= 0.0f) {
            e.active = false;
        }
    }
}

void ExplosionManager::draw() {
    for (auto &e : m_pool) {
        if (!e.active) continue;

        float alpha = e.timer / e.duration;
        unsigned char a = (unsigned char)(alpha * 255);

        for (int i = 0; i < e.particleCount; i++) {
            Color c = e.color;
            c.a = a;
            DrawCircleV(e.particles[i].pos, e.particles[i].radius, c);
        }
    }
}

void ExplosionManager::reset() {
    for (auto &e : m_pool) {
        e.active = false;
    }
}
