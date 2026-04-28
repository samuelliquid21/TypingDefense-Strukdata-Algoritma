#include "AsteroidManager.h"

// fungsi helper

// LINKED LIST ASTEROID SHOWER

void newAsteroid(AsteroidShowerData *&head) {
    AsteroidShowerData *newAst = new AsteroidShowerData();
    newAst->next = nullptr;

    if (head == nullptr) {
        head = newAst;
        return;
    }

    newAst->next = head;
    head = newAst;
}

void deleteAllAsteroid(AsteroidShowerData *&head) {
    AsteroidShowerData *current = head;
    while (current != nullptr) {
        AsteroidShowerData *next = current->next;   
        delete current;                              
        current = next;   
    }                           
    head = nullptr;   
}

void AsteroidManager::executeShowerWave() {
    if (current == nullptr) {
        current = asteroidShowerHead;
    }

    AsteroidShowerTime += GetFrameTime();

    if (AsteroidShowerTime >= 0.2f) {
        AsteroidShowerTime = 0;
        if (current != nullptr) {
            current->asteroids.asteroidType(2);
            current = current->next;
        }
    }
}

Asteroid *AsteroidManager::checkAsteroidShower(char charTyped) {
    AsteroidShowerData *current = asteroidShowerHead; 
    while (current != nullptr)
    {
        if (current->asteroids.active && current->asteroids.word[0] == charTyped) {
            return &current->asteroids;
        }
    }
}

// CIRCULAR QUEUE ASTEROID BIASA

void AsteroidManager::activateAsteroid(int diff) {
    asteroid[asteroidFront].asteroidType(diff);
    asteroidFront++;
    if (asteroidFront == 20) {
        asteroidFront = 0;
    }
}

Asteroid *AsteroidManager::checkAsteroid(char charTyped) {
    Asteroid *current = nullptr;
    for (auto &ast : asteroid) {
        if (ast.active && ast.word[0] == charTyped) {
            if (current == nullptr) {
                current = &ast;
            } else if (ast.position.x > current->position.x) {
                current = &ast;
            }
        }
    }
    return current;
}

// CIRCULAR PRIORITY QUEUE UNTUK EVENT HANDLING

void AsteroidManager::enqueue() {

}