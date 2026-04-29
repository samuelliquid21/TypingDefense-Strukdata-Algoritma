#include "SpaceShip.h"
#include "raylib.h"

SpaceShip::SpaceShip() {}

SpaceShip::~SpaceShip() {
    UnloadTexture(texture);
}

void SpaceShip::init() {
    texture = LoadTexture("./assets/img/Spaceships.png");
    
    // --- TAMBAHKAN KODE DEBUG INI ---
    if (texture.id == 0) {
        // Jika masuk ke sini, berarti file gambar TIDAK DITEMUKAN
        TraceLog(LOG_ERROR, "GAMBAR SPACESHIP GAGAL DI-LOAD! Cek path file kamu.");
    } else {
        // Jika berhasil, kita cek ukurannya
        TraceLog(LOG_INFO, "GAMBAR BERHASIL DI-LOAD! Ukuran: %d x %d", texture.width, texture.height);
    }
    // --------------------------------

    int columns = 5;
    int rows = 3;
    
    // Jika gagal load (width 0), kita cegah pembagian dengan 0 yang aneh
    if (texture.width > 0 && texture.height > 0) {
        float frameWidth = (float)texture.width / columns;
        float frameHeight = (float)texture.height / rows;

        int selectedColumn = 2; 
        int selectedRow = 1;    

        frame = { 
            selectedColumn * frameWidth, 
            selectedRow * frameHeight, 
            frameWidth, 
            frameHeight 
        };
    }
}

void SpaceShip::activateLaser(Vector2 &target) {
    laserTarget = target;
    laserIsActive = true;
    laserNextStop = GetTime() + LASER_DELAY;
}

void SpaceShip::deactivateLaser() {
    if (GetTime() >= laserNextStop) {
        laserIsActive = false;
    }
}

void SpaceShip::drawLaser() {
    if (laserIsActive) {
        DrawLineEx(position, laserTarget, 3, BLUE);
    }
}

bool SpaceShip::asteroiHitSpaceship(Vector2 &asteroidPosition, int asteroidRadius) {
    return CheckCollisionCircles(asteroidPosition, asteroidRadius, position, radiusHitbox);
}

void SpaceShip::drawSpaceShip() {
    // DrawCircleV(position, radiusHitbox, RED);
    float scale = 2.5f;
    Rectangle destRec = { position.x, position.y, frame.width * scale, frame.height * scale};
    Vector2 origin = { (frame.width * scale) / 2.0f, (frame.height * scale) / 2.0f };
    DrawTexturePro(
        texture,    // Tekstur sumber
        frame,      // Bagian yang dipotong dari spritesheet
        destRec,    // Posisi dan ukuran di layar
        origin,     // Titik pusat (agar presisi dengan position)
        180.0f,       // Sudut rotasi (0 = tidak miring)
        WHITE       // Warna tint aslinya
    );
}

void SpaceShip::update(float deltaTime) {
    deactivateLaser();
}

void SpaceShip::draw() {
    drawLaser();
    drawSpaceShip();
}