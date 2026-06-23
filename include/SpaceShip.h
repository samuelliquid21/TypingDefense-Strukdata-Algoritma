#pragma once

#include "raylib.h"
#include "GameConfig.h"

constexpr float LASER_DELAY = 0.1f; // Durasi laser menyala sebelum mati otomatis (detik)

// Kelas spaceship pemain: posisi, texture, dan sistem laser
class SpaceShip
{
private:
    Vector2 position{Config::playerStartPos}; // Posisi spaceship di layar (tetap)
    bool hitPlayer{false};                    // Flag apakah sedang terkena asteroid

    // Sistem laser
    Vector2 laserTarget{0, 0};    // Posisi target laser saat ini
    bool laserIsActive{false};    // Flag apakah laser sedang menyala
    double laserNextStop{0};      // Waktu (GetTime) kapan laser harus mati

    Texture2D texture;  // Texture sprite spaceship
    Rectangle frame;    // Area sprite yang diambil dari spritesheet

    void drawTexture();     // Gambar texture spaceship
    void deactivateLaser(); // Matikan laser jika sudah waktunya
    void drawLaser();       // Gambar garis laser ke target
    void drawSpaceShip();   // Gambar spaceship di posisi saat ini

public:
    SpaceShip();
    ~SpaceShip();       // Unload texture di destructor
    void init();        // Load texture dari file, setup frame sprite

    void activateLaser(Vector2 &target); // Aktifkan laser dan arahkan ke target
    void update(float deltaTime);        // Update logika (deactivate laser)
    void draw();                         // Render laser + spaceship
};
