#include "SpaceShip.h"
#include "AsteroidManager.h"

enum typingState {
    SEARCH_FOR_TARGET,
    TARGET_LOCKED
};

class GameplayManager
{
private:
    SpaceShip spaceship;
    AsteroidManager asteroidManager;
    typingState state = typingState::SEARCH_FOR_TARGET;
    Asteroid* currentTarget = nullptr;
public:
    int score = 0;

    bool isHit();

    void update(float deltaTime);
    void draw();
    void reset();
};