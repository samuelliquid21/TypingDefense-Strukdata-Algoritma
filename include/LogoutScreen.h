#pragma once
#include "raylib.h"

class LogoutScreen {
public:
    LogoutScreen();
    void Update();
    void Draw();
    void Reset();
    
    bool IsFinished() const;

private:
    float m_logoutTimer;
    bool m_isFinished;
};