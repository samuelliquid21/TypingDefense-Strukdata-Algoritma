#include "SpaceShip.h"
#include "AsteroidManager.h"

enum typingState {
    SEARCH_FOR_TARGET,
    TARGET_LOCKED
};

class GamplayManager
{
private:
    SpaceShip spaceship;
    AsteroidManager asteoridManager;
public:
    int score = 0;

    void Update(float delteTime);
    void draw();
};