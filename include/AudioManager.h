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

    // === Convenience (lobby/credit/bgm) ===
    void UpdateLobby();
    void UpdateLobbyNoSeek();
    void UpdateCredit();
    void UpdateDefault();
    void StopLobby();
    void StopCredit();

private:
    AudioManager() = default;
    ~AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void loadMusicTracks(); // Load semua music tracks
    void loadSoundEffects(); // Load semua sound effects
    void setDefaultVolumes(); // Set volume default untuk SFX dan music
};
