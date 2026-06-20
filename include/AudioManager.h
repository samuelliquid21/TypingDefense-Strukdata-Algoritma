#pragma once
#include "raylib.h"

class AudioManager {
public:
    AudioManager() = default;
    ~AudioManager();

    void Init();

    void UpdateLobby();
    void UpdateLobbyNoSeek();
    void UpdateCredit();

    void StopLobby();
    void StopCredit();

private:
    Music m_lobby{};
    Music m_credit{};
    bool m_loaded = false;
};
