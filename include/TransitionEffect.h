#pragma once
#include "State.h"
#include "raylib.h"

class TransitionEffect {
public:
    TransitionEffect() = default;
    ~TransitionEffect();

    void Init();

    void Update(float dt);
    void Draw() const;

    void Start(GameState target);

    bool IsActive() const;
    GameState GetTargetState() const;

    void PlaySoundIn();
    void PlaySoundOut();

private:
    float m_timer = 0.0f;
    float m_duration = 0.6f;
    bool m_active = false;
    float m_intensity = 0.0f;
    GameState m_target = GameState::MENU;
    bool m_loaded = false;

    Sound m_soundIn{};
    Sound m_soundOut{};
};
