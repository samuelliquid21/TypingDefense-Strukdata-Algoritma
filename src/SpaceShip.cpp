#include "SpaceShip.h"
#include "SkinData.h"
#include "raylib.h"

SpaceShip::SpaceShip() {}

SpaceShip::~SpaceShip() {
    UnloadTexture(texture);
}

void SpaceShip::init() {
    texture = LoadTexture("./assets/img/Spaceships.png");
    
    if (texture.id == 0) {
        TraceLog(LOG_ERROR, "GAMBAR SPACESHIP GAGAL DI-LOAD! Cek path file kamu.");
    } else {
        TraceLog(LOG_INFO, "GAMBAR BERHASIL DI-LOAD! Ukuran: %d x %d", texture.width, texture.height);
    }

    currentSkinId = 5;
    setSkin(currentSkinId);
}

void SpaceShip::setSkin(int skinId) {
    if (texture.id == 0) return;
    if (skinId < 0 || skinId >= 15) return;

    currentSkinId = skinId;
    const SkinInfo& info = getSkinInfo(skinId);

    float frameWidth = (float)texture.width / GRID_COLS;
    float frameHeight = (float)texture.height / GRID_ROWS;

    frame = {
        info.col * frameWidth,
        info.row * frameHeight,
        frameWidth,
        frameHeight
    };
}

// mengaktifkan laser ketika dan mengarahkannya ke target
void SpaceShip::activateLaser(Vector2 &target) {
    laserTarget = target;
    laserIsActive = true;
    laserNextStop = GetTime() + LASER_DELAY;
}

// mematikan laser
void SpaceShip::deactivateLaser() {
    if (GetTime() >= laserNextStop) {
        laserIsActive = false;
    }
}

// fungsi untuk menggambar laser (menggunakan draw line)
void SpaceShip::drawLaser() {
    if (laserIsActive) {
        DrawLineEx(position, laserTarget, 3, BLUE);
    }
}

// cek apakah ada asteroid yang menabrak spaceship
bool SpaceShip::asteroiHitSpaceship(Vector2 &asteroidPosition, int asteroidRadius) {
    return CheckCollisionCircles(asteroidPosition, asteroidRadius, position, radiusHitbox);
}

// menggambar texture spaceship
void SpaceShip::drawSpaceShip() {
    float scale = 2.5f;
    Rectangle destRec = { position.x, position.y, frame.width * scale, frame.height * scale};
    Vector2 origin = { (frame.width * scale) / 2.0f, (frame.height * scale) / 2.0f };
    DrawTexturePro(
        texture,    
        frame,      
        destRec,    
        origin,     
        180.0f,      
        WHITE       
    );
}

// logika game

void SpaceShip::update(float deltaTime) {
    deactivateLaser();
}

void SpaceShip::draw() {
    drawLaser();
    drawSpaceShip();
}