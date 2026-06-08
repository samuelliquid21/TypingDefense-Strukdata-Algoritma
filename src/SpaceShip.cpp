#include "SpaceShip.h"
#include "raylib.h"

SpaceShip::SpaceShip() {}

SpaceShip::~SpaceShip() {
    // Bebaskan memory texture saat objek dihancurkan
    UnloadTexture(texture);
}

void SpaceShip::init() {
    // Load texture spritesheet spaceship
    texture = LoadTexture("./assets/img/Spaceships.png");

    // Validasi: pastikan texture berhasil di-load
    if (texture.id == 0) {
        TraceLog(LOG_ERROR, "GAMBAR SPACESHIP GAGAL DI-LOAD! Cek path file kamu.");
    } else {
        TraceLog(LOG_INFO, "GAMBAR BERHASIL DI-LOAD! Ukuran: %d x %d", texture.width, texture.height);
    }

    // Ambil satu frame dari spritesheet 5x3
    int columns = 5;
    int rows = 3;

    if (texture.width > 0 && texture.height > 0) {
        float frameWidth = (float)texture.width / columns;
        float frameHeight = (float)texture.height / rows;

        // Pilih sprite kolom 2, baris 1 (sesuai desain)
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

// Cek tabrakan antara asteroid dan spaceship (collision circle-circle)
bool SpaceShip::asteroiHitSpaceship(Vector2 &asteroidPosition, int asteroidRadius) {
    return CheckCollisionCircles(asteroidPosition, asteroidRadius, position, radiusHitbox);
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
