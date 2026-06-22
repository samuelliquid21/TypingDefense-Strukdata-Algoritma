#include "Timer.h"

// ===============================
// ⏱️ TIMER UTILITY
// ===============================

// Mulai timer: set durasi baru, reset waktu berjalan, aktifkan
void Timer::start (float dur) {
    duration = dur;
    lifeTime = 0.0f;
    active = true;
}

// Update timer dengan delta time: tambahkan ke lifeTime, nonaktifkan jika sudah mencapai durasi
void Timer::update(float deltaTime) {
    if (active) {
        lifeTime += deltaTime;
        if (lifeTime >= duration) {
            active = false;
        }
    }
}

// Cek apakah timer sudah selesai (inaktif DAN lifetime sudah mencapai durasi)
bool Timer::isDone() const {
    return !active && lifeTime >= duration;
}

// Dapatkan progress dalam rentang 0.0 - 1.0; return 1.0 jika duration 0 (hindari NaN)
float Timer::getProgress() const {
    if (duration == 0) return 1.0;
    return (lifeTime / duration);
}
