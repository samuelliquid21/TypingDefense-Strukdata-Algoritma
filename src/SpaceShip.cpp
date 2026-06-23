#include "SpaceShip.h"
#include "AssetManager.h"
#include "SkinManager.h"
#include "raylib.h"

// ===============================
// 🚀 SPACESHIP
// ===============================

SpaceShip::SpaceShip() {}

SpaceShip::~SpaceShip() {
    // no-op: AssetManager handle lifecycle
}

void SpaceShip::init() {
    auto& skinMgr = SkinManager::getInstance();
    texture = skinMgr.getSpritesheet();
    const SkinInfo& info = getSkinInfo(skinMgr.getActiveSkin());
    currentSkinId = skinMgr.getActiveSkin();
    float fw = (float)texture.width / GRID_COLS;
    float fh = (float)texture.height / GRID_ROWS;
    frame = { info.col * fw, info.row * fh, fw, fh };
}

// Aktifkan laser dan arahkan ke target (posisi asteroid)
void SpaceShip::activateLaser(Vector2 &target) {
    laserTarget = target;
    laserIsActive = true;
    laserNextStop = GetTime() + LASER_DELAY; // Set timer mati otomatis
}

// Matikan laser jika sudah mencapai batas waktu
void SpaceShip::deactivateLaser() {
    if (GetTime() >= laserNextStop) {
        laserIsActive = false;
    }
}

// Gambar garis laser dari spaceship ke target
void SpaceShip::drawLaser() {
    if (laserIsActive) {
        DrawLineEx(position, laserTarget, 3, BLUE);
    }
}

// Gambar spaceship dengan texture dari spritesheet
void SpaceShip::drawSpaceShip() {
    float scale = 2.5f;
    Rectangle destRec = { position.x, position.y, frame.width * scale, frame.height * scale };
    Vector2 origin = { (frame.width * scale) / 2.0f, (frame.height * scale) / 2.0f };
    DrawTexturePro(
        texture,    // Texture spritesheet
        frame,      // Source: area sprite yang dipilih
        destRec,    // Dest: posisi dan ukuran di layar
        origin,     // Origin: pusat rotasi
        180.0f,     // Rotasi 180 derajat (menghadap ke kiri)
        WHITE       // Warna tint
    );
}

void SpaceShip::update(float deltaTime) {
    // Cek dan matikan laser jika timeout
    deactivateLaser();
}

void SpaceShip::draw() {
    drawLaser();
    drawSpaceShip();
}

void SpaceShip::setSkin(int skinId) {
    currentSkinId = skinId;
    auto& skinMgr = SkinManager::getInstance();
    texture = skinMgr.getSpritesheet();
    const SkinInfo& info = getSkinInfo(skinId);
    float fw = (float)texture.width / GRID_COLS;
    float fh = (float)texture.height / GRID_ROWS;
    frame = { info.col * fw, info.row * fh, fw, fh };
}

int SpaceShip::getSkinId() const {
    return currentSkinId;
}
