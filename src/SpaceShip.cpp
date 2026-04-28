#include "SpaceShip.h"
#include "raylib.h"

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
    DrawCircleV(position, radiusHitbox, RED);
}

char SpaceShip::getCharPressed() {
    return pressed;
}

void SpaceShip::update(float deltaTime) {
    pressed = GetCharPressed();
    deactivateLaser();
}

void SpaceShip::draw() {
    drawLaser();
    drawSpaceShip();
    DrawText(TextFormat("%c", pressed), 100, 100, 20, WHITE);
}