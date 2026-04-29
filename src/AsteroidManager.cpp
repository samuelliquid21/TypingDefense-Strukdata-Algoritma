#include "AsteroidManager.h"
#include "raylib.h"
#include "GameConfig.h"

AsteroidManager::AsteroidManager() {
    // Inisialisasi circular array buffer untuk asteroid pooling (size 20)
    for (int i = 0; i < 20; i++) {
        asteroid[i].active = false;
    }
    asteroidFront = 0;  // Front index untuk dequeuing
    asteroidBack = 0;   // Back index untuk enqueuing
    asteroidCount = 0;  // Count asteroid aktif

    // Inisialisasi priority queue circular buffer (size 10)
    for (int i = 0; i < 10; i++) {
        eventQueue[i].valid = false;
        eventQueue[i].type = 0;
        eventQueue[i].priority = 0;
    }
    eventFront = 0;
    eventBack = 0;
    eventCount = 0;
    eventNormalTime = 0.0f;
    eventShowerTime = 0.0f;
    eventExecuteTime = 0.0f;
    eventAddNode = 0.0f;

    initShowerLinkedList();  // Inisialisasi singly linked list untuk asteroid shower
}

AsteroidManager::~AsteroidManager() {
    // Cleanup memory untuk singly linked list asteroid shower
    AsteroidShowerData* current = asteroidShowerHead;
    while (current != nullptr) {
        AsteroidShowerData* next = current->next;
        delete current;
        current = next;
    }
    asteroidShowerHead = nullptr;
}

void AsteroidManager::clear() {
    AsteroidShowerData* temp = asteroidShowerHead;
    while (temp != nullptr) {
        AsteroidShowerData* next = temp->next;
        delete temp;
        temp = next;
    }
    asteroidShowerHead = nullptr;
    this->current = nullptr;
    asteroidFront = 0;
    asteroidBack = 0;
    asteroidCount = 0;
    eventFront = 0;
    eventBack = 0;
    eventCount = 0;
    eventNormalTime = 0.0f;
    eventShowerTime = 0.0f;
    eventExecuteTime = 0.0f;
    eventAddNode = 0.0f;
    showerActivateTime = 0.0f;
    difficultyManager.resetTime();
    for (int i = 0; i < 20; i++) {
        asteroid[i].active = false;
    }
    for (int i = 0; i < 10; i++) {
        eventQueue[i].valid = false;
        eventQueue[i].type = 0;
        eventQueue[i].priority = 0;
    }
    initShowerLinkedList();
}

void AsteroidManager::initShowerLinkedList() {
    int count = difficultyManager.getAsteroidCountForSpecialSpawn();
    for (int i = 0; i < count; i++) {
        addShowerNode();
    }
    showerActivateTime = 0.0f;
}

void AsteroidManager::addShowerNode() {
    AsteroidShowerData* newNode = new AsteroidShowerData();
    newNode->asteroids.active = false;
    newNode->next = nullptr;
    if (asteroidShowerHead == nullptr) {
        asteroidShowerHead = newNode;
    } else {
        newNode->next = asteroidShowerHead;
        asteroidShowerHead = newNode;
    }
}

void AsteroidManager::executeShowerWave() {
    current = asteroidShowerHead;
    showerActivateTime = 0.0f;
}

Asteroid* AsteroidManager::checkAsteroidShower(char charTyped) {
    Asteroid* closest = nullptr;
    AsteroidShowerData* temp = asteroidShowerHead;

    while (temp != nullptr) {
        if (temp->asteroids.active && !temp->asteroids.word.empty() &&
            temp->asteroids.word[0] == charTyped) {
            if (closest == nullptr || temp->asteroids.position.x > closest->position.x) {
                closest = &temp->asteroids;
            }
        }
        temp = temp->next;
    }
    return closest;
}

// Aktifkan asteroid baru di circular array buffer
// Menggunakan circular indices dengan modulo 20 untuk wrap-around
void AsteroidManager::activateAsteroid(int diff) {
    if (asteroidCount >= 20) {
        // Buffer penuh: cari slot yang tidak aktif untuk reuse
        int idx = asteroidFront;
        for (int i = 0; i < 20; i++) {
            if (!asteroid[idx].active) {
                asteroid[idx].asteroidType(diff);
                asteroidFront = (idx + 1) % 20;  // Update front dengan modulo
                return;
            }
            idx = (idx + 1) % 20;  // Circular increment
        }
        return;
    }

    // Buffer belum penuh: tambah di back position
    asteroid[asteroidBack].asteroidType(diff);
    asteroidBack = (asteroidBack + 1) % 20;  // Increment back dengan modulo
    asteroidCount++;
}

Asteroid* AsteroidManager::checkAsteroid(char charTyped) {
    Asteroid* closest = nullptr;

    for (int i = 0; i < 20; i++) {
        if (asteroid[i].active && !asteroid[i].word.empty() &&
            asteroid[i].word[0] == charTyped) {
            if (closest == nullptr || asteroid[i].position.x > closest->position.x) {
                closest = &asteroid[i];
            }
        }
    }
    return closest;
}

// Tambah event ke priority queue (circular buffer size 10)
// Menggunakan modulo 10 untuk circular indices
void AsteroidManager::enqueueEvent(int type, int priority) {
    if (eventCount >= 10) return;

    int idx = eventBack;
    while (eventQueue[idx].valid) {
        idx = (idx + 1) % 10;  // Circular search untuk slot kosong
    }

    eventQueue[idx].type = type;
    eventQueue[idx].priority = priority;
    eventQueue[idx].valid = true;
    eventBack = idx;
    eventCount++;
}

// Ambil event dengan priority tertinggi dari queue
// Linear search O(n) untuk priority tertinggi
PriorityEvent AsteroidManager::dequeueEvent() {
    PriorityEvent emptyEvent;
    emptyEvent.valid = false;
    emptyEvent.type = 0;
    emptyEvent.priority = 0;

    if (eventCount == 0) return emptyEvent;

    int highestIdx = -1;
    int highestPriority = 0;

    for (int i = 0; i < 10; i++) {
        if (eventQueue[i].valid) {
            if (highestIdx == -1 || eventQueue[i].priority > highestPriority) {
                highestPriority = eventQueue[i].priority;
                highestIdx = i;
            }
        }
    }

    if (highestIdx == -1) return emptyEvent;

    PriorityEvent result = eventQueue[highestIdx];
    eventQueue[highestIdx].valid = false;
    eventCount--;

    return result;
}

void AsteroidManager::executeEvent(const PriorityEvent& event) {
    if (!event.valid) return;

    if (event.type == ASTEROID_SHOWER) {
        executeShowerWave();
    } else if (event.type == NORMAL) {
        int tier = difficultyManager.getAsteroidTier();
        activateAsteroid(tier);
    }
}

Asteroid* AsteroidManager::getTarget(char charTyped) {
    Asteroid* showerResult = checkAsteroidShower(charTyped);
    Asteroid* normalResult = checkAsteroid(charTyped);

    if (showerResult != nullptr && normalResult != nullptr) {
        if (showerResult->position.x >= normalResult->position.x) {
            return showerResult;
        } else {
            return normalResult;
        }
    }

    if (showerResult != nullptr) return showerResult;
    if (normalResult != nullptr) return normalResult;

    return nullptr;
}

bool AsteroidManager::collisionWithPlayer() {
    bool collisionStatus = false;
    for(auto &ast : asteroid) {
        if (ast.active) {
            collisionStatus = CheckCollisionCircles(ast.position, ast.radius, Config::playerStartPos, Config::playerHitbox);
            if (collisionStatus) {
                return collisionStatus;
            }
        }
    }
    AsteroidShowerData *currentAst = asteroidShowerHead;
    while (currentAst != nullptr)
    {
        if (currentAst->asteroids.active) {
            collisionStatus = CheckCollisionCircles(currentAst->asteroids.position, currentAst->asteroids.radius, Config::playerStartPos, Config::playerHitbox);
            if (collisionStatus) {
                return collisionStatus;
            }
        }
        currentAst = currentAst->next;
    }
    
    return collisionStatus;
}

void AsteroidManager::update(float deltaTime) {
    difficultyManager.updateTime();

    for (int i = 0; i < 20; i++) {
        if (asteroid[i].active) {
            asteroid[i].update(deltaTime);
        }
    }

    AsteroidShowerData* temp = asteroidShowerHead;
    while (temp != nullptr) {
        if (temp->asteroids.active) {
            temp->asteroids.update(deltaTime);
        }
        temp = temp->next;
    }

    eventNormalTime += deltaTime;
    eventShowerTime += deltaTime;
    eventExecuteTime += deltaTime;
    eventAddNode += deltaTime;

    if (eventNormalTime >= 2.0f) {
        enqueueEvent(NORMAL, 1);
        eventNormalTime = 0.0f;
    }

    if (eventShowerTime >= 30.0f) {
        enqueueEvent(ASTEROID_SHOWER, 2);
        eventShowerTime = 0.0f;
    }

    if (eventExecuteTime >= 1.0f) {
        eventExecuteTime = 0.0f;
        PriorityEvent evt = dequeueEvent();
        if (evt.valid) {
            executeEvent(evt);
        }
    }

    if (eventAddNode >= 60.0f) {
        addShowerNode();
        eventAddNode = 0.0f;
    }

    if (current != nullptr) {
        showerActivateTime += deltaTime;
        if (showerActivateTime >= 0.2f) {
            showerActivateTime = 0.0f;
            current->asteroids.asteroidType(GetRandomValue(1, 2));
            current = current->next;
        }
    }
}

void AsteroidManager::draw() {
    for (int i = 0; i < 20; i++) {
        if (asteroid[i].active) {
            asteroid[i].draw();
        }
    }

    AsteroidShowerData* temp = asteroidShowerHead;
    while (temp != nullptr) {
        if (temp->asteroids.active) {
            temp->asteroids.draw();
        }
        temp = temp->next;
    }

    DrawText(std::to_string(difficultyManager.counter).c_str(), 50, 50, 20, WHITE);
}