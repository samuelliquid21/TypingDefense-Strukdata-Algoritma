#include "BombSkill.h"
#include "raylib.h"
#include "GameConfig.h"

BombSkill::BombSkill() {}

// Aktifkan bom: mulai animasi shockwave, return false jika masih cooldown
bool BombSkill::activate() {
    if (state != IDLE) return false; // Abaikan jika sedang meledak atau cooldown
    state = EXPLODING;
    timer = 0.0f;
    shockwaveRadius = 0.0f;
    return true;
}

// Update: animasi shockwave membesar, lalu transisi ke COOLDOWN
void BombSkill::update(float deltaTime) {
    if (state == EXPLODING) {
        timer += deltaTime;
        // Radius membesar linear dari 0 sampai maxRadius selama durasi ledakan
        shockwaveRadius = (timer / Config::bombShockwaveDuration) * Config::bombShockwaveMaxRadius;
        // Ledakan selesai → mulai cooldown
        if (timer >= Config::bombShockwaveDuration) {
            state = COOLDOWN;
            timer = 0.0f;
        }
    } else if (state == COOLDOWN) {
        timer += deltaTime;
        // Cooldown selesai → kembali IDLE
        if (timer >= Config::bombCooldown) {
            state = IDLE;
            timer = 0.0f;
        }
    }
}

// Render lingkaran shockwave — memudar (alpha menurun) seiring radius membesar
void BombSkill::draw() {
    if (state != EXPLODING) return;

    // Alpha: 1.0 di awal → 0.0 di akhir ledakan
    float alpha = 1.0f - (timer / Config::bombShockwaveDuration);
    Color c1 = {255, 255, 255, (unsigned char)(alpha * 180)};     // Lingkaran luar (putih)
    Color c2 = {0, 200, 255, (unsigned char)(alpha * 80)};        // Lingkaran dalam (cyan)

    DrawCircleLines(
        Config::playerStartPos.x,
        Config::playerStartPos.y,
        shockwaveRadius,
        c1
    );
    DrawCircleLines(
        Config::playerStartPos.x,
        Config::playerStartPos.y,
        shockwaveRadius * 0.8f,
        c2
    );
}

const char* BombSkill::getName() const {
    return "Bomb";
}

bool BombSkill::isReady() const {
    return state == IDLE;
}

bool BombSkill::isActive() const {
    return state == EXPLODING;
}

// Getter untuk deteksi collision asteroid dengan shockwave
float BombSkill::getShockwaveRadius() const {
    return shockwaveRadius;
}

// Progress cooldown: 0.0 saat IDLE/EXPLODING, 0.0–1.0 saat COOLDOWN
float BombSkill::getCooldownProgress() const {
    if (state == IDLE) return 0.0f;
    if (state == EXPLODING) return 0.0f;
    return timer / Config::bombCooldown;
}
