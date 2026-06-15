#pragma once

#include "raylib.h"
#include "GameConfig.h"

#define LASER_DELAY 0.1f

class SpaceShip
{
private:
    Vector2 position{Config::playerStartPos};
    int radiusHitbox{30};
    bool hitPlayer{false};

    Vector2 laserTarget{0, 0};
    bool laserIsActive{false};
    double laserNextStop{0};

    Texture2D texture;
    Rectangle frame;
    int currentSkinId{0};

    void drawTexture();
    void deactivateLaser();
    void drawLaser();
    void drawSpaceShip();

public:
    SpaceShip();
    ~SpaceShip();
    void init();
    void setSkin(int skinId);
    int getSkinId() const { return currentSkinId; }
    bool asteroiHitSpaceship(Vector2 &asteroidPosition, int asteroidRadius);
    void activateLaser(Vector2 &target);
    void update(float deltaTime);
    void draw();
};



