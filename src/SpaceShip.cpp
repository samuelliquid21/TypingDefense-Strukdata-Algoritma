#include "SpaceShip.h"
#include "raylib.h"

SpaceShip::SpaceShip() {}

SpaceShip::~SpaceShip() {
    UnloadTexture(texture);
}

void SpaceShip::init() {
    texture = LoadTexture("./assets/img/Spaceships.png");
    
    // check apakah texture ke load atau tidak
    if (texture.id == 0) {
        TraceLog(LOG_ERROR, "GAMBAR SPACESHIP GAGAL DI-LOAD! Cek path file kamu.");
    } else {
        TraceLog(LOG_INFO, "GAMBAR BERHASIL DI-LOAD! Ukuran: %d x %d", texture.width, texture.height);
    }

    // posisi icon di sprite
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