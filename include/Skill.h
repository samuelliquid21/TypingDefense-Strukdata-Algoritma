#pragma once

class Skill {
public:
    virtual ~Skill() = default;
    virtual void update(float deltaTime) = 0;
    virtual void draw() = 0;
    virtual const char* getName() const = 0;
    virtual bool isReady() const = 0;
    virtual bool isActive() const = 0;
    virtual float getCooldownProgress() const = 0;
};
