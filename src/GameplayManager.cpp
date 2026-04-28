#include "GameplayManager.h"

bool GameplayManager::isHit() {
    return asteroidManager.collisionWithPlayer();
}

void GameplayManager::update(float deltaTime) {
    spaceship.update(deltaTime);
    asteroidManager.update(deltaTime);

    char c = GetCharPressed();
    if (c == '\0') return;

    if (state == SEARCH_FOR_TARGET) {
        currentTarget = asteroidManager.getTarget(c);
        if (currentTarget != nullptr) {
            currentTarget->targeted = true;
            spaceship.activateLaser(currentTarget->position);
            int result = currentTarget->typingAsteroid(c);
            if (result > 0) {
                score += result;
                state = TARGET_LOCKED;
            }
        }
    } else if (state == TARGET_LOCKED) {
        if (currentTarget == nullptr || !currentTarget->active) {
            state = SEARCH_FOR_TARGET;
            currentTarget = nullptr;
            return;
        }
        int result = currentTarget->typingAsteroid(c);
        if (result > 0) {
            score += result;
            if (currentTarget->word.empty()) {
                state = SEARCH_FOR_TARGET;
                currentTarget = nullptr;
            } else {
                spaceship.activateLaser(currentTarget->position);
            }
        }
    }
}

void GameplayManager::draw() {
    spaceship.draw();
    asteroidManager.draw();
}

void GameplayManager::reset() {
    score = 0;
    state = typingState::SEARCH_FOR_TARGET;
    currentTarget = nullptr;
    spaceship = SpaceShip();
    asteroidManager.clear();
}