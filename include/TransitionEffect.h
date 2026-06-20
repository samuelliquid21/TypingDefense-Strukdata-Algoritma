#pragma once
#include "State.h"
#include "raylib.h"

class TransitionEffect {
public:
    TransitionEffect();
    ~TransitionEffect();

    void Update(float dt);
    void Draw() const;

    void Start(GameState target);

    bool IsActive() const;
    GameState GetTargetState() const;

    void PlaySoundIn();
    void PlaySoundOut();

private:
    float m_timer;
    float m_duration;
    bool m_active;
    float m_intensity;
    GameState m_target;

    Sound m_soundIn;
    Sound m_soundOut;

    void loadSounds();
    void unloadSounds();
};
