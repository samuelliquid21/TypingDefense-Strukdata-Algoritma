#include "Timer.h"

void Timer::start (float dur) { // memulai waktu
    duration = dur;
    lifeTime = 0.0f;
    active = true;
}

void Timer::update(float deltaTime) { // update waktu (taruh di game logic bagian update)
    if (active) {
        lifeTime += deltaTime;
        if (lifeTime >= duration) {
            active = false;
        }
    }
}

bool Timer::isDone() const { // cek apakah timer sudah selesai
    return !active && lifeTime >= duration;
}

float Timer::getProgress() const { // cek progress timer (berguna untuk animasi)
    if (duration == 0) return 1.0;
    return (lifeTime / duration);
}