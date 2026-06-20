#pragma once
#include <string>

class AudioManager {
public:
    static AudioManager& getInstance();

    void Init();

    // === Generic Music API ===
    void playMusic(const std::string& key);
    void stopMusic(const std::string& key);
    void updateMusic(const std::string& key);
    void setMusicVolume(const std::string& key, float vol);

    // === Generic SFX API ===
    void playSfx(const std::string& key);
    void playSfxOnce(const std::string& key);
    void setSfxVolume(const std::string& key, float vol);

    // === Convenience (lobby/credit) ===
    void UpdateLobby();
    void UpdateLobbyNoSeek();
    void UpdateCredit();
    void StopLobby();
    void StopCredit();

private:
    AudioManager() = default;
    ~AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
};
