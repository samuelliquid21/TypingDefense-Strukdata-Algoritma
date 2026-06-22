#include "AudioManager.h"
#include "AssetManager.h"
#include "raylib.h"

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

void AudioManager::Init() {
    // Music
    AssetManager::getInstance().loadMusic("lobby", "assets/sound/soundtrack.mp3");
    AssetManager::getInstance().loadMusic("credit", "assets/sound/cosmic.mp3");
    AssetManager::getInstance().loadMusic("bgmLeaderboard", "assets/sound/bgm.mp3");
    AssetManager::getInstance().loadMusic("bgm", "assets/sound/bgm.mp3");

    // SFX
    AssetManager::getInstance().loadSound("glitchMasuk", "assets/sound/glitchmasuk.mp3");
    AssetManager::getInstance().loadSound("glitchKeluar", "assets/sound/glitchkeluar.mp3");
    AssetManager::getInstance().loadSound("click", "assets/sound/click.mp3");
    AssetManager::getInstance().loadSound("laser", "assets/sound/laser.mp3");
    AssetManager::getInstance().loadSound("error", "assets/sound/error.mp3");
    AssetManager::getInstance().loadSound("gameover", "assets/sound/gameover.mp3");
    AssetManager::getInstance().loadSound("explosion", "assets/sound/explosion.mp3");

    // Default volumes
    setSfxVolume("laser", 0.4f);
    setSfxVolume("error", 0.5f);
    setSfxVolume("gameover", 0.8f);
    setSfxVolume("explosion", 1.0f);

    setMusicVolume("lobby", 0.5f);
    setMusicVolume("credit", 0.5f);
    setMusicVolume("bgm", 0.5f);

    // Start lobby
    playMusic("lobby");
    SeekMusicStream(AssetManager::getInstance().getMusic("lobby"), 5.0f);
}

// ===============================
// GENERIC MUSIC
// ===============================

void AudioManager::playMusic(const std::string& key) {
    Music mus = AssetManager::getInstance().getMusic(key);
    if (mus.stream.buffer != nullptr)
        PlayMusicStream(mus);
}

void AudioManager::stopMusic(const std::string& key) {
    Music mus = AssetManager::getInstance().getMusic(key);
    if (mus.stream.buffer != nullptr)
        StopMusicStream(mus);
}

void AudioManager::updateMusic(const std::string& key) {
    Music mus = AssetManager::getInstance().getMusic(key);
    if (mus.stream.buffer != nullptr)
        UpdateMusicStream(mus);
}

void AudioManager::setMusicVolume(const std::string& key, float vol) {
    Music mus = AssetManager::getInstance().getMusic(key);
    if (mus.stream.buffer != nullptr)
        SetMusicVolume(mus, vol);
}

// ===============================
// GENERIC SFX
// ===============================

void AudioManager::playSfx(const std::string& key) {
    Sound snd = AssetManager::getInstance().getSound(key);
    if (snd.stream.buffer != nullptr)
        PlaySound(snd);
}

void AudioManager::playSfxOnce(const std::string& key) {
    Sound snd = AssetManager::getInstance().getSound(key);
    if (snd.stream.buffer != nullptr && !IsSoundPlaying(snd))
        PlaySound(snd);
}

void AudioManager::setSfxVolume(const std::string& key, float vol) {
    Sound snd = AssetManager::getInstance().getSound(key);
    if (snd.stream.buffer != nullptr)
        SetSoundVolume(snd, vol);
}

// ===============================
// CONVENIENCE (LOBBY / CREDIT)
// ===============================

void AudioManager::UpdateLobby() {
    Music mus = AssetManager::getInstance().getMusic("lobby");
    if (mus.stream.buffer == nullptr) return;
    UpdateMusicStream(mus);
    if (!IsMusicStreamPlaying(mus)) {
        PlayMusicStream(mus);
        SeekMusicStream(mus, 5.0f);
    }
}

void AudioManager::UpdateLobbyNoSeek() {
    Music mus = AssetManager::getInstance().getMusic("lobby");
    if (mus.stream.buffer == nullptr) return;
    UpdateMusicStream(mus);
    if (!IsMusicStreamPlaying(mus)) {
        PlayMusicStream(mus);
    }
}

void AudioManager::UpdateCredit() {
    Music mus = AssetManager::getInstance().getMusic("credit");
    if (mus.stream.buffer == nullptr) return;
    UpdateMusicStream(mus);
    if (!IsMusicStreamPlaying(mus)) {
        PlayMusicStream(mus);
        SeekMusicStream(mus, 5.0f);
    }
}

void AudioManager::StopLobby() {
    stopMusic("lobby");
}

void AudioManager::StopCredit() {
    stopMusic("credit");
}

void AudioManager::UpdateDefault() {
    Music mus = AssetManager::getInstance().getMusic("bgm");
    if (mus.stream.buffer == nullptr) return;
    UpdateMusicStream(mus);
    if (!IsMusicStreamPlaying(mus)) {
        PlayMusicStream(mus);
    }
}
