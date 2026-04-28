#include "Asteroid.h"
#include "DifficultyManager.h"

// Event
enum EventType {
    NORMAL = 1,
    ASTEROID_SHOWER
};

// struct asteroid shower
struct AsteroidShowerData {
    Asteroid asteroids;
    AsteroidShowerData *next;
};

// struct event handler
struct Event {
    int events[10];
    int size{10};
    int Front{-1};
    int Back{-1};
};


class AsteroidManager
{
private:
    // difficulty manager
    DifficultyManager difficultyManager;

    // Asteroid biasa circular queue
    Asteroid asteroid[20];
    int asteroidFront = 0;
    void activateAsteroid(int diff);
    Asteroid *checkAsteroid(char charTyped);


    // Asteroid Shower linked list
    AsteroidShowerData *asteroidShowerHead{nullptr};
    AsteroidShowerData *current{nullptr};
    float AsteroidShowerTime{0.0f};
    void executeShowerWave();
    Asteroid *checkAsteroidShower(char charTyped);

    // Event handling
    Event event;
    float eventNormalTime{0.0f};
    float eventShowerTime{0.0f};
    void enqueue();
    void dequeue();
    int pickEvent();

public:
    // CONSTRUCTOR DAN DESTRUCTOR
    AsteroidManager();
    ~AsteroidManager();

    // METHOD UTAMA
    Asteroid *getTarget(char charTyped); // memberi alamat asteroid ketika char yang diketik benar

    // METHOD
    void update(float deltaTime);
    void draw();
};