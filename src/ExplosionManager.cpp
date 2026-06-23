#include "ExplosionManager.h"
#include "raymath.h"

// ===============================
// 💥 MANAGER LEDAKAN
// ===============================

// Inisialisasi partikel ledakan dengan arah dan kecepatan random
static void initExplosionParticles(Explosion& e, Vector2 pos) {
    for (int i = 0; i < e.particleCount; i++) {
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float speed = (float)GetRandomValue(80, 300);
        e.particles[i].pos = pos;
        e.particles[i].vel = { cosf(angle) * speed, sinf(angle) * speed };
        e.particles[i].radius = (float)GetRandomValue(3, 8);
    }
}

// Spawn partikel ledakan di posisi tertentu.
// Mencari slot pool yang tidak aktif, lalu menginisialisasi
// partikel dengan arah random dan kecepatan 80-300.
void ExplosionManager::spawn(Vector2 pos, int count, Color color) {
    for (auto &e : m_pool) {
        if (!e.active) {
            e.pos = pos;
            e.color = color;
            e.timer = e.duration;
            e.particleCount = (count > 35) ? 35 : count; // Maks 35 partikel per ledakan
            e.active = true;

            // Init tiap partikel dengan arah dan kecepatan random
            initExplosionParticles(e, pos);
            return;
        }
    }
}

// Update posisi partikel (bergerak sesuai velocity) dengan damping.
// Partikel yang sudah habis durasi dinonaktifkan.
void ExplosionManager::update(float dt) {
    for (auto &e : m_pool) {
        if (!e.active) continue;

        e.timer -= dt;
        // Gerak partikel + perlambatan
        for (int i = 0; i < e.particleCount; i++) {
            e.particles[i].pos.x += e.particles[i].vel.x * dt;
            e.particles[i].pos.y += e.particles[i].vel.y * dt;
            e.particles[i].vel.x *= 0.98f; // Damping
            e.particles[i].vel.y *= 0.98f;
        }

        if (e.timer <= 0.0f) {
            e.active = false;
        }
    }
}

// Render semua partikel ledakan yang aktif.
// Alpha memudar seiring sisa durasi (timer/duration).
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


