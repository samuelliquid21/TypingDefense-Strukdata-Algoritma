#include "AudioManager.h"

AudioManager::~AudioManager() {
    if (m_loaded) {
        UnloadMusicStream(m_lobby);
        UnloadMusicStream(m_credit);
    }
}

void AudioManager::Init() {
    m_lobby = LoadMusicStream("assets/sound/soundtrack.mp3");
    SetMusicVolume(m_lobby, 0.5f);

    m_credit = LoadMusicStream("assets/sound/cosmic.mp3");
    SetMusicVolume(m_credit, 0.5f);

    PlayMusicStream(m_lobby);
    SeekMusicStream(m_lobby, 5.0f);

    m_loaded = true;
}

void AudioManager::UpdateLobby() {
    UpdateMusicStream(m_lobby);
    if (!IsMusicStreamPlaying(m_lobby)) {
        PlayMusicStream(m_lobby);
        SeekMusicStream(m_lobby, 5.0f);
    }
}

void AudioManager::UpdateLobbyNoSeek() {
    UpdateMusicStream(m_lobby);
    if (!IsMusicStreamPlaying(m_lobby)) {
        PlayMusicStream(m_lobby);
    }
}

void AudioManager::UpdateCredit() {
    UpdateMusicStream(m_credit);
    if (!IsMusicStreamPlaying(m_credit)) {
        PlayMusicStream(m_credit);
        SeekMusicStream(m_credit, 5.0f);
    }
}

void AudioManager::StopLobby() {
    StopMusicStream(m_lobby);
}

void AudioManager::StopCredit() {
    StopMusicStream(m_credit);
}
