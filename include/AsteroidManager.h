#pragma once
#include "Asteroid.h"
#include "DifficultyManager.h"

enum EventType {
    NORMAL = 1,
    ASTEROID_SHOWER = 2
};

struct AsteroidShowerData {
    Asteroid asteroids;
    AsteroidShowerData *next;
};

struct PriorityEvent {
    int type;
    int priority;
    bool valid;
};

class AsteroidManager
{
private:
    DifficultyManager difficultyManager;

    Asteroid asteroid[20];
    int asteroidFront = 0;
    int asteroidBack = 0;
    int asteroidCount = 0;
    void activateAsteroid(int diff);
    Asteroid* checkAsteroid(char charTyped);

    AsteroidShowerData* asteroidShowerHead = nullptr;
    AsteroidShowerData* current = nullptr;
    float showerActivateTime = 0.0f;
    void initShowerLinkedList();
    void addShowerNode();
    void executeShowerWave();
    Asteroid* checkAsteroidShower(char charTyped);

    PriorityEvent eventQueue[10];
    int eventFront = 0;
    int eventBack = 0;
    int eventCount = 0;
    float eventNormalTime = 0.0f;
    float eventShowerTime = 0.0f;
    float eventExecuteTime = 0.0f;
    float eventAddNode = 0.0f;
    void enqueueEvent(int type, int priority);
    PriorityEvent dequeueEvent();
    void executeEvent(const PriorityEvent& event);

public:
    AsteroidManager();
    ~AsteroidManager();

    Asteroid* getTarget(char charTyped);
    void update(float deltaTime);
    void draw();
};